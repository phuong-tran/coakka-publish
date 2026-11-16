"""Verify standalone HTTP/3 file drain with a controlled final ACK."""

import asyncio
from contextlib import AsyncExitStack
from dataclasses import dataclass, field
from pathlib import Path
import sys
import tempfile

from aioquic.asyncio import QuicConnectionProtocol, connect
from aioquic.h3.connection import H3_ALPN, H3Connection
from aioquic.h3.events import DataReceived, HeadersReceived
from aioquic.quic.configuration import QuicConfiguration
from aioquic.quic.events import ConnectionTerminated

from stream_wire import require


@dataclass
class FileResponse:
    headers: dict = field(default_factory=dict)
    body: bytearray = field(default_factory=bytearray)
    finished: asyncio.Event = field(default_factory=asyncio.Event)


class FilePeer(QuicConnectionProtocol):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.http = H3Connection(self._quic)
        self.responses = {}
        self.closed = asyncio.Event()
        self.silent = False
        self.first_stream = None
        self.close_code = None

    def transmit(self):
        if not self.silent:
            super().transmit()

    def datagram_received(self, data, address):
        super().datagram_received(data, address)
        if self.silent:
            # A receive-only peer still runs the closing timer. The base
            # protocol would normally arm it from its transmit path.
            if self._timer is not None:
                self._timer.cancel()
            expiry = self._quic.get_timer()
            self._timer_at = expiry
            self._timer = None if expiry is None else self._loop.call_at(expiry, self._handle_timer)

    def resume(self):
        self.silent = False
        self.transmit()

    def quic_event_received(self, event):
        if isinstance(event, ConnectionTerminated):
            self.close_code = event.error_code
            self.closed.set()
        for message in self.http.handle_event(event):
            response = self.responses[message.stream_id]
            if isinstance(message, HeadersReceived):
                response.headers.update(message.headers)
            if isinstance(message, DataReceived):
                response.body.extend(message.data)
                if message.stream_ended:
                    if message.stream_id == self.first_stream:
                        # The base protocol transmits after event delivery.
                        # Hold the ACK for the datagram carrying the first FIN.
                        self.silent = True
                    response.finished.set()

    def request_range(self, start, end):
        stream = self._quic.get_next_available_stream_id()
        response = FileResponse()
        self.responses[stream] = response
        if self.first_stream is None:
            self.first_stream = stream
        self.http.send_headers(stream, [
            (b":method", b"GET"),
            (b":scheme", b"https"),
            (b":authority", b"localhost"),
            (b":path", b"/download"),
            (b"range", f"bytes={start}-{end}".encode()),
        ], end_stream=True)
        self.transmit()
        return response


async def verify_range(response, asset, start, end):
    await asyncio.wait_for(response.finished.wait(), 4)
    require(response.headers.get(b":status") == b"206", f"Bad status: {response.headers}")
    content_range = f"bytes {start}-{end}/{len(asset)}".encode()
    require(response.headers.get(b"content-range") == content_range,
            f"Wrong Content-Range: {response.headers}")
    require(response.body == asset[start:end + 1], f"Wrong file bytes: {response.body}")


async def exercise(mode, certificate, key, scratch, consumer, ca=None):
    with tempfile.TemporaryDirectory(dir=scratch, prefix="http-file-drain-") as root:
        asset = bytes(range(256)) * 192
        (Path(root) / "asset.txt").write_bytes(asset)
        (Path(root) / "index.html").write_bytes(b"<p>SPA fixture</p>")
        owner = await asyncio.create_subprocess_exec(
            consumer, "3", certificate, key, root,
            "--sibling" if mode == "sibling" else "--one-file",
            stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
        )

        async def marker():
            return (await asyncio.wait_for(owner.stdout.readline(), 7)).decode().strip()

        async def send(command):
            owner.stdin.write(command + b"\n")
            await owner.stdin.drain()

        try:
            ready = await marker()
            require(ready.startswith("READY "), f"Unexpected listener state: {ready}")
            port = int(ready.removeprefix("READY "))
            config = QuicConfiguration(is_client=True, alpn_protocols=H3_ALPN,
                                       server_name="localhost", cafile=ca or certificate)
            async with AsyncExitStack() as cleanup:
                peer = await cleanup.enter_async_context(connect(
                    "127.0.0.1", port, configuration=config, create_protocol=FilePeer,
                ))
                cleanup.callback(peer.resume)
                await asyncio.wait_for(peer.ping(), 3)
                await send(b"check")
                require(await marker() == "STATIC", "Unexpected request before the file case")
                first = peer.request_range(2, 5)
                await send(b"file")
                if mode == "sibling":
                    require(await marker() == "ADMITTED", "Core did not admit the first file")
                    sibling = peer.request_range(7, 10)
                await verify_range(first, asset, 2, 5)
                require(await marker() == "SENT", "Missing response terminal")
                await send(b"stop")
                require(await marker() == "DRAINING", "Core failed to start drain")
                if mode == "sibling":
                    require(await marker() == "PENDING", "Drain retired the live sibling")
                    peer.resume()
                    await verify_range(sibling, asset, 7, 10)
                    require(await marker() == "SIBLING", "Missing sibling terminal")
                    print("Sibling remained admitted while the first ACK was held and delivered its 206 and FIN",
                          flush=True)
                if mode == "ack":
                    peer.resume()
                result = await marker()
                expected = "EXPIRED" if mode == "expire" else "COMPLETE"
                require(result.startswith(f"DRAIN {expected} "), f"Wrong outcome: {result}")
                print(result, flush=True)
                if mode == "expire":
                    await asyncio.wait_for(peer.closed.wait(), 2)
                    require(peer.close_code == 0x10C, f"Wrong close code: {peer.close_code}")
                require(await marker() == f"REBOUND {port}", "Failed to reopen the UDP port")
            require(await asyncio.wait_for(owner.wait(), 4) == 0,
                    "Native consumer failed during stop or destroy")
        finally:
            if owner.returncode is None:
                owner.kill()
                await owner.wait()


if __name__ == "__main__":
    asyncio.run(exercise(*sys.argv[1:]))
