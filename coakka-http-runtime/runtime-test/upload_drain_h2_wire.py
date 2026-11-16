"""Drive a standalone HTTP/2 upload across the native drain deadline."""

import argparse
import asyncio
from pathlib import Path
import ssl

from h2.config import H2Configuration
from h2.connection import H2Connection


async def marker(owner, expected):
    observed = await asyncio.wait_for(owner.stdout.readline(), 4)
    if observed.rstrip(b"\r\n") != expected:
        raise RuntimeError(f"Expected {expected!r}, got {observed!r}")


async def run(options, mode, payload):
    owner = await asyncio.create_subprocess_exec(
        str(options.consumer), mode, payload, str(options.certificate),
        str(options.key), options.backend, "h2", stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )
    writer = None
    try:
        ready = await asyncio.wait_for(owner.stdout.readline(), 4)
        if not ready.startswith(b"PORT="):
            raise RuntimeError(f"Missing native port: {ready!r}")
        port = int(ready.removeprefix(b"PORT="))
        context = ssl.create_default_context()
        context.check_hostname = False
        context.verify_mode = ssl.CERT_NONE  # Loopback fixture certificate.
        context.set_alpn_protocols(["h2"])
        _, writer = await asyncio.wait_for(asyncio.open_connection(
            "127.0.0.1", port, ssl=context), 4)
        if writer.get_extra_info("ssl_object").selected_alpn_protocol() != "h2":
            raise RuntimeError("Standalone consumer did not negotiate HTTP/2")
        peer = H2Connection(config=H2Configuration(client_side=True))
        peer.initiate_connection()
        stream = peer.get_next_available_stream_id()
        body = b"a" * 900
        headers = [
            (b":method", b"POST"), (b":scheme", b"https"),
            (b":authority", b"localhost"), (b":path", b"/upload"),
            (b"content-length", b"900" if payload == "trailers" else b"24000"),
        ]
        if payload == "trailers":
            headers.append((b"trailer", b"x-receipt"))
        peer.send_headers(stream, headers)
        writer.write(peer.data_to_send())
        await writer.drain()
        await marker(owner, b"HEAD")

        peer.send_data(stream, body)
        writer.write(peer.data_to_send())
        await writer.drain()
        if payload == "trailers":
            await marker(owner, b"DATA")
            peer.send_headers(stream, [(b"x-receipt", b"ready")], end_stream=True)
            writer.write(peer.data_to_send())
            await writer.drain()

        output, error = await asyncio.wait_for(owner.communicate(), 10)
        print(output.decode(), end="")
        if owner.returncode or b'"cleanup":true' not in output:
            raise RuntimeError(error.decode())
    finally:
        if owner.returncode is None:
            owner.kill()
            await owner.communicate()
        if writer is not None:
            writer.close()


async def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("consumer", type=Path)
    parser.add_argument("certificate", type=Path)
    parser.add_argument("key", type=Path)
    parser.add_argument("--backend", choices=("auto", "io-uring"), default="auto")
    parser.add_argument("--repeat", type=int, default=1)
    options = parser.parse_args()
    for _ in range(options.repeat):
        await run(options, "returned", "data")
        for mode in ("expiry", "abort"):
            for payload in ("data", "trailers"):
                await run(options, mode, payload)


if __name__ == "__main__":
    asyncio.run(main())
