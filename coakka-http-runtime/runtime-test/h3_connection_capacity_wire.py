"""Compare the installed standalone HTTP/3 connection limit with live peers."""

import argparse
import asyncio
from contextlib import AsyncExitStack
from pathlib import Path
import socket
import ssl

from aioquic.asyncio import QuicConnectionProtocol, connect
from aioquic.h3.connection import H3_ALPN, H3Connection
from aioquic.h3.events import DataReceived, HeadersReceived
from aioquic.quic.configuration import QuicConfiguration
from aioquic.quic.events import ConnectionTerminated, ProtocolNegotiated
from aioquic.quic.logger import QuicLogger
from aioquic.quic.packet import QuicErrorCode


class Peer(QuicConnectionProtocol):
    def __init__(self, connection, **kwargs):
        super().__init__(connection, **kwargs)
        self.http = None
        self.termination = None
        self.reply = asyncio.get_running_loop().create_future()
        self.status = None
        self.body = bytearray()

    def quic_event_received(self, event):
        if isinstance(event, ProtocolNegotiated):
            self.http = H3Connection(self._quic)
        elif isinstance(event, ConnectionTerminated):
            self.termination = event.error_code
        if self.http is None:
            return
        for message in self.http.handle_event(event):
            if isinstance(message, HeadersReceived):
                self.status = dict(message.headers).get(b":status", self.status)
            elif isinstance(message, DataReceived):
                self.body.extend(message.data)
            if isinstance(message, (HeadersReceived, DataReceived)) and message.stream_ended:
                self.reply.set_result((self.status, bytes(self.body)))

    def request(self):
        stream = self._quic.get_next_available_stream_id()
        self.http.send_headers(stream, [
            (b":method", b"GET"),
            (b":scheme", b"https"),
            (b":authority", b"localhost"),
            (b":path", b"/capacity"),
        ], end_stream=True)
        self.transmit()


async def read_line(host):
    line = await asyncio.wait_for(host.stdout.readline(), 5)
    assert line, f"Installed owner exited with {host.returncode}"
    return line.decode().strip()


async def command(host, value, expected):
    host.stdin.write(f"{value}\n".encode())
    await host.stdin.drain()
    actual = await read_line(host)
    assert actual == expected, (expected, actual)


def peer_context(port, root, logger=None, observed=None):
    configuration = QuicConfiguration(
        is_client=True, alpn_protocols=H3_ALPN, server_name="localhost",
        verify_mode=ssl.CERT_REQUIRED, cafile=str(root), quic_logger=logger,
    )

    def create(connection, **kwargs):
        peer = Peer(connection, **kwargs)
        if observed is not None:
            observed.append(peer)
        return peer

    return connect("127.0.0.1", port, configuration=configuration, create_protocol=create)


async def expect_refused(port, root, stack):
    logger = QuicLogger()
    observed = []
    try:
        await asyncio.wait_for(stack.enter_async_context(
            peer_context(port, root, logger, observed)), 4)
    except ConnectionError:
        (peer,) = observed
        assert peer.termination == QuicErrorCode.CONNECTION_REFUSED, peer.termination
        close_frames = [
            frame for trace in logger.to_dict()["traces"]
            for event in trace["events"]
            if event["name"] == "transport:packet_received"
            and event["data"]["header"]["packet_type"] == "initial"
            for frame in event["data"].get("frames", [])
            if frame["frame_type"] == "connection_close"
        ]
        assert any(frame["error_space"] == "transport"
                   and frame["error_code"] == QuicErrorCode.CONNECTION_REFUSED
                   for frame in close_frames), close_frames
        return
    raise AssertionError("The QUIC connection beyond the accepted capacity succeeded")


async def finish_reply(host, peer):
    await command(host, "reply", "TERMINAL")
    assert await asyncio.wait_for(peer.reply, 5) == (b"200", b"accepted")
    assert peer.termination is None


async def run(arguments, requested, expected, active):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as reserve:
        reserve.bind(("127.0.0.1", 0))
        port = reserve.getsockname()[1]
    host = await asyncio.create_subprocess_exec(
        arguments.executable, str(port), str(requested), str(expected), str(active),
        arguments.cert, arguments.key, arguments.backend,
        stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
    )
    try:
        assert await read_line(host) == f"READY {port} connections={expected} active={active}"
        async with AsyncExitStack() as peers:
            first_context = peer_context(port, arguments.root)
            async with first_context as first:
                first.request()
                await command(host, "take", "ADMITTED")
                survivors = [await asyncio.wait_for(peers.enter_async_context(
                    peer_context(port, arguments.root)), 4) for _ in range(expected - 1)]
                await expect_refused(port, arguments.root, peers)
                await finish_reply(host, first)
                # Prove the listener kept a sibling alive while at its ceiling.
                survivors[0].request()
                await command(host, "take", "ADMITTED")
                await finish_reply(host, survivors[0])
            # The first context has closed. Keep every sibling open while a
            # fresh peer uses the single slot returned to the UDP owner.
            replacement = await asyncio.wait_for(peers.enter_async_context(
                peer_context(port, arguments.root)), 4)
            replacement.request()
            await command(host, "take", "ADMITTED")
            await finish_reply(host, replacement)
        await command(host, "stop", "STOPPED AND REOPENED")
        assert await asyncio.wait_for(host.wait(), 5) == 0
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as reopened:
            reopened.bind(("127.0.0.1", port))
        print(f"ABI 10 {arguments.backend} requested={requested} accepted={expected} "
              f"active={active} refused={expected + 1} replacement/reopen PASS", flush=True)
    finally:
        if host.returncode is None:
            host.kill()
            await host.wait()


async def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for item in ("executable", "cert", "key", "root"):
        parser.add_argument(item, type=Path)
    parser.add_argument("--backend", choices=("default", "uring"), default="default")
    parser.add_argument("--requested", type=int, choices=(0, 3, 8, 9, 64))
    args = parser.parse_args()
    for requested, capacity, active in ((8, 8, 1), (64, 8, 16), (0, 8, 1),
                                         (3, 3, 1), (9, 8, 1)):
        if args.requested is None or requested == args.requested:
            await run(args, requested, capacity, active)


if __name__ == "__main__":
    asyncio.run(main())
