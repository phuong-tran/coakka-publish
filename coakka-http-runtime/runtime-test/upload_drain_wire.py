"""Drive ordinary localhost HTTP/3 uploads and check native lease retirement."""

import argparse
import asyncio
from pathlib import Path
import ssl

from aioquic.asyncio import QuicConnectionProtocol, connect
from aioquic.h3.connection import H3_ALPN, H3Connection
from aioquic.quic.configuration import QuicConfiguration


class Peer(QuicConnectionProtocol):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.http = H3Connection(self._quic)

    def quic_event_received(self, event):
        self.http.handle_event(event)

    def start_upload(self, payload_kind):
        stream = self._quic.get_next_available_stream_id()
        body = b"a" * 900
        headers = [
            (b":method", b"POST"), (b":scheme", b"https"),
            (b":authority", b"localhost"), (b":path", b"/upload"),
        ]
        headers.append((b"content-length",
                        b"24000" if payload_kind == "data" else b"900"))
        if payload_kind == "trailers":
            headers.append((b"trailer", b"x-receipt"))
        self.http.send_headers(stream, headers)
        self.transmit()
        return stream, body

    def send_body(self, stream, body):
        self.http.send_data(stream, body, end_stream=False)
        self.transmit()

    def send_trailers(self, stream):
        self.http.send_headers(stream, [(b"x-receipt", b"ready")], end_stream=True)
        self.transmit()


async def run(options, mode, payload_kind):
    owner = await asyncio.create_subprocess_exec(
        str(options.consumer), mode, payload_kind, str(options.certificate),
        str(options.key), options.backend, stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )
    try:
        first = await asyncio.wait_for(owner.stdout.readline(), 6)
        if not first.startswith(b"PORT="):
            raise RuntimeError(f"Missing native port: {first!r}")
        port = int(first.removeprefix(b"PORT="))
        config = QuicConfiguration(is_client=True, alpn_protocols=H3_ALPN)
        config.verify_mode = ssl.CERT_NONE  # Local disposable fixture.
        async with connect("127.0.0.1", port, configuration=config,
                           create_protocol=Peer) as peer:
            stream, body = peer.start_upload(payload_kind)
            head = await asyncio.wait_for(owner.stdout.readline(), 6)
            if head.rstrip(b"\r\n") != b"HEAD":
                output, error = await asyncio.wait_for(owner.communicate(), 3)
                raise RuntimeError(f"Missing request head: {head!r} {output!r} {error!r}")
            peer.send_body(stream, body)
            if payload_kind == "trailers":
                data = await asyncio.wait_for(owner.stdout.readline(), 6)
                if data.rstrip(b"\r\n") != b"DATA":
                    raise RuntimeError(f"Missing DATA: {data!r}")
                peer.send_trailers(stream)
            output, error = await asyncio.wait_for(owner.communicate(), 10)
            print(output.decode(), end="")
            if owner.returncode != 0 or b'"cleanup":true' not in output:
                raise RuntimeError(error.decode())
    finally:
        if owner.returncode is None:
            owner.kill()
            await owner.communicate()


async def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("consumer", type=Path)
    parser.add_argument("certificate", type=Path)
    parser.add_argument("key", type=Path)
    parser.add_argument("--backend", choices=("auto", "io-uring"), default="auto")
    parser.add_argument("--repeat", type=int, default=1)
    args = parser.parse_args()
    for _ in range(args.repeat):
        for mode in ("expiry", "abort"):
            for payload in ("data", "trailers"):
                await run(args, mode, payload)


if __name__ == "__main__":
    asyncio.run(main())
