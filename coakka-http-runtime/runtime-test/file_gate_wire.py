"""Check static file bytes, early disconnect, recovery and port reuse."""

import argparse
import asyncio
from pathlib import Path
import socket
import ssl
import tempfile


FILE = bytes(range(256)) * 32768
PATH = b"/assets/asset.bin"


async def get(port, context, headers=b""):
    reader, writer = await asyncio.wait_for(asyncio.open_connection(
        "127.0.0.1", port, ssl=context,
        server_hostname="localhost" if context else None), 5)
    writer.write(b"GET " + PATH + b" HTTP/1.1\r\nHost: localhost\r\n"
                 b"Connection: close\r\n" + headers + b"\r\n")
    await writer.drain()
    head = await asyncio.wait_for(reader.readuntil(b"\r\n\r\n"), 5)
    return reader, writer, head


async def exercise(options):
    context = None
    if options.mode == "tls":
        context = ssl.create_default_context(cafile=str(options.certificate))
        context.set_alpn_protocols(["http/1.1"])
    with tempfile.TemporaryDirectory(prefix="file-gate-", dir=options.scratch) as work:
        file_path = Path(work) / "asset.bin"
        file_path.write_bytes(FILE)
        with socket.socket() as probe:
            probe.bind(("127.0.0.1", 0))
            port = probe.getsockname()[1]
        owner = await asyncio.create_subprocess_exec(
            str(options.consumer), options.mode, str(options.certificate),
            str(options.key), work, str(port), stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE)
        try:
            ready = await asyncio.wait_for(owner.stdout.readline(), 7)
            assert ready.startswith(b"PORT="), ready
            assert int(ready[5:]) == port, ready
            for index in range(3):
                reader, writer, head = await get(port, context)
                assert head.startswith(b"HTTP/1.1 200 "), head
                assert b"\r\ncontent-length: 8388608\r\n" in head.lower(), head
                assert await asyncio.wait_for(reader.readexactly(1024), 5) == FILE[:1024]
                # Retire the connection while a large transfer is still in flight.
                writer.transport.abort()
                await asyncio.wait_for(writer.wait_closed(), 5)

                start = 1234 + index
                end = start + 383
                request_range = f"Range: bytes={start}-{end}\r\n".encode()
                reader, writer, head = await get(port, context, request_range)
                try:
                    assert head.startswith(b"HTTP/1.1 206 "), head
                    assert (f"\r\ncontent-range: bytes {start}-{end}/{len(FILE)}\r\n".encode()
                            in head.lower()), head
                    actual = await asyncio.wait_for(reader.read(), 5)
                    assert actual == FILE[start:end + 1], (len(actual), start)
                finally:
                    writer.close()
                    await asyncio.wait_for(writer.wait_closed(), 5)
            owner.stdin.write(b"\n")
            await owner.stdin.drain()
            output, _ = await asyncio.wait_for(owner.communicate(), 8)
            assert owner.returncode == 0 and b'"cleanup":true' in output, output
            # Windows refuses deletion while certain file handles remain open.
            file_path.unlink()
            print(f"PASS file {options.mode}: 3 prefix disconnects, exact ranges, drain and port reuse")
        finally:
            if owner.returncode is None:
                owner.kill()
                await owner.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("consumer", type=Path)
    parser.add_argument("certificate", type=Path)
    parser.add_argument("key", type=Path)
    parser.add_argument("scratch", type=Path)
    parser.add_argument("mode", choices=("plain", "tls"))
    asyncio.run(exercise(parser.parse_args()))


if __name__ == "__main__":
    main()
