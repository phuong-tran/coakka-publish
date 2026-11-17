"""Pair an installed C ABI probe with an independent HTTP peer for drain."""
import asyncio
from pathlib import Path
import ssl
import sys

from lifecycle_http3_peer import h3_peer
from lifecycle_interactions_wire import h2_peer


async def line(process, prefix):
    text = (await asyncio.wait_for(process.stdout.readline(), 10)).decode().strip()
    print(text, flush=True)
    if not text.startswith(prefix):
        raise AssertionError((prefix, text))
    return text


async def http1(port, trust, tls):
    context = ssl.create_default_context(cafile=str(trust)) if tls else None
    reader, writer = await asyncio.wait_for(asyncio.open_connection(
        "127.0.0.1", port, ssl=context,
        server_hostname="localhost" if tls else None,
    ), 5)
    try:
        writer.write(b"GET /held HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
        await writer.drain()
        head = await asyncio.wait_for(reader.readuntil(b"\r\n\r\n"), 6)
        body = await asyncio.wait_for(reader.readexactly(len(b"held-response")), 5)
        assert head.startswith(b"HTTP/1.1 200 ") and body == b"held-response", (head, body)
    finally:
        # The runtime may close TLS after delivering the complete response.
        # The native side verifies drain and shutdown independently.
        writer.close()


async def run(exe, cert, key, trust, profile, backend, monitor):
    process = await asyncio.create_subprocess_exec(
        str(exe), profile, backend, monitor, str(cert), str(key),
        stdout=asyncio.subprocess.PIPE,
    )
    try:
        assert "sequence=1 admission=1 active=0" in await line(process, "IDLE ok ")
        assert "sequence=2 admission=1 active=0" in await line(process, "IDLE ok ")
        port = int((await line(process, "READY ")).split()[1])
        if profile.startswith("h1"):
            await http1(port, trust, profile == "h1-tls")
        else:
            peer_owner = h2_peer(port, trust) if profile == "h2" else h3_peer(port, trust, 8192)
            async with peer_owner as peer:
                stream = await peer.request(b"/held")
                await peer.verify(stream, b"held-response")
        assert "sequence=3 admission=0 active=1" in await line(process, "DRAIN ok ")
        await line(process, f"PASS {profile} {backend} {monitor} ")
        result = await line(process, "{")
        assert '"cleanup":true' in result, result
        assert await asyncio.wait_for(process.wait(), 5) == 0
    finally:
        if process.returncode is None:
            process.kill()
            await process.wait()


async def main():
    executable, certificate, key, ca = map(Path, sys.argv[1:5])
    profile, backend, monitor = sys.argv[5:]
    await run(executable, certificate, key, ca, profile, backend, monitor)


if __name__ == "__main__":
    asyncio.run(main())
