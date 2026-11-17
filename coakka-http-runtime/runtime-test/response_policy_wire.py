"""Verify installed response policy using independent loopback HTTP peers."""

import argparse
import asyncio
from contextlib import asynccontextmanager, suppress
import gzip
import http.client
from pathlib import Path
import re
import socket

SLOW_DATA_LIMIT = 32768
SIBLING_FIRST = b"data: sibling-first\n\n"
SIBLING_LAST = b"data: sibling-last\n\n"


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


class Http1Stream:
    """Read just enough chunked response bytes to prove incremental delivery."""

    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.body = bytearray()

    async def request(self, path):
        self.writer.write(b"GET " + path + b" HTTP/1.1\r\nHost: localhost\r\n\r\n")
        await self.writer.drain()
        lines = (await self.reader.readuntil(b"\r\n\r\n")).split(b"\r\n")
        require(lines[0].startswith(b"HTTP/1.1 200 "), f"Unexpected head: {lines[0]!r}")
        headers = dict(line.lower().split(b": ", 1) for line in lines[1:] if b": " in line)
        require(headers.get(b"transfer-encoding") == b"chunked", f"Missing chunk framing: {headers}")
        sse_headers(headers)

    async def chunk(self):
        size = int((await self.reader.readline()).strip(), 16)
        require(size <= 8192, f"Unexpected SSE chunk size: {size}")
        block = await self.reader.readexactly(size + 2)
        require(block.endswith(b"\r\n"), "Incomplete chunk delimiter")
        self.body.extend(block[:-2])
        return size

    async def prefix(self, body):
        while len(self.body) < len(body):
            require(await self.chunk() != 0, "SSE finished before sending the prefix")
        require(self.body.startswith(body), f"SSE prefix mismatch: {self.body!r}")

    async def finish(self, body):
        while await self.chunk():
            require(len(self.body) <= len(body), "Too much sibling SSE data")
        require(self.body == body, f"Unexpected sibling SSE: {self.body!r}")


@asynccontextmanager
async def h1_stream(port):
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.setblocking(False)
    connection.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 4096)
    try:
        await asyncio.get_running_loop().sock_connect(connection, ("127.0.0.1", port))
        reader, writer = await asyncio.open_connection(sock=connection, limit=1024)
    except BaseException:
        connection.close()
        raise
    try:
        yield Http1Stream(reader, writer)
    finally:
        writer.close()
        with suppress(ConnectionError):
            await writer.wait_closed()


def h1_get(port, path, headers):
    client = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        client.putrequest("GET", path.decode(), skip_accept_encoding=True)
        for name, value in headers:
            client.putheader(name.decode(), value.decode())
        client.endheaders()
        response = client.getresponse()
        require(response.version == 11, "Expected HTTP/1.1")
        return (str(response.status).encode(),
                {k.lower().encode(): v.encode() for k, v in response.getheaders()},
                response.read(8193))
    finally:
        client.close()


async def get_response(peer, port, path, headers=()):
    if peer is None:
        return await asyncio.to_thread(h1_get, port, path, headers)
    stream = await peer.request(path, extra_headers=headers)
    response = peer.responses[stream]
    await peer.until(lambda: response.ended or response.reset)
    require(not response.reset, f"Response reset unexpectedly on stream {stream}")
    return response.status, response.headers, bytes(response.body)


def sse_headers(headers):
    require(headers.get(b"content-type", b"").split(b";")[0] == b"text/event-stream",
            f"Missing SSE content type: {headers}")


async def slow_sse_h1(port, control, expire):
    async with h1_stream(port) as slow, h1_stream(port) as sibling:
        await slow.request(b"/stream")
        await slow.prefix(b"data: slow-first\n\n")
        slow.writer.transport.pause_reading()
        await control.pressure("fill", expect_write=True)
        await sibling.request(b"/events")
        await sibling.prefix(SIBLING_FIRST)
        require((await get_response(None, port, b"/ordinary"))[::2] == (b"200", b"ordinary-ok"),
                "Ordinary HTTP/1.1 response did not complete")
        await sibling.finish(SIBLING_FIRST + SIBLING_LAST)
        await control.pressure("bound")
        if expire:
            await control.expect("WRITE_TIMEOUT")
            require((await get_response(None, port, b"/ordinary"))[::2] == (b"200", b"ordinary-ok"),
                    "Ordinary HTTP/1.1 response failed after the writer timeout")
        else:
            slow.writer.transport.abort()
            await control.send("cancelled")


async def sse_prefix(peer, stream, expected):
    response = peer.responses[stream]
    await peer.until(lambda: len(response.body) >= len(expected) or response.reset)
    sse_headers(response.headers)
    require(response.status == b"200" and response.body == expected and
            not response.ended and not response.reset, f"SSE did not send a live prefix: {response}")


async def slow_sse_multiplexed(peer, protocol, port, control):
    slow = await peer.request(b"/stream")
    await sse_prefix(peer, slow, b"data: slow-first\n\n")
    await control.pressure("fill", expect_write=True)
    sibling = await peer.request(b"/events")
    await sse_prefix(peer, sibling, SIBLING_FIRST)
    require((await get_response(peer, port, b"/ordinary"))[::2] == (b"200", b"ordinary-ok"),
            "Ordinary request did not complete on the held stream's connection")
    await peer.verify(sibling, SIBLING_FIRST + SIBLING_LAST)
    pending = peer.responses[slow]
    require(not pending.ended and not pending.reset and len(pending.body) <= 1024,
            f"The held stream exceeded its credit or retired early: {len(pending.body)}")
    await control.pressure("bound")
    if protocol == "3":
        # End the response direction of a request that already sent FIN.
        peer._quic.stop_stream(slow, 0x10C)
        peer.transmit()
    else:
        await peer.reset(slow)
    await control.send("cancelled")


async def gzip_fallback(peer, port):
    body = b"a" * 768
    cases = (
        (b"/gzip", b"gzip", b"200", body, True),
        (b"/gzip", b"gzip;q=0, identity;q=1", b"200", body, False),
        # An omitted field allows Core's enabled gzip policy to select gzip.
        (b"/gzip", None, b"200", body, True),
        (b"/small", b"gzip", b"200", b"small", False),
        (b"/empty", b"gzip", b"204", b"", False),
    )
    for path, encoding, expected_status, expected_body, encoded in cases:
        request_headers = ((b"accept-encoding", encoding),) if encoding is not None else ()
        status, headers, payload = await get_response(peer, port, path, request_headers)
        require(status == expected_status, f"{path!r} returned HTTP status {status!r}")
        if status == b"204":
            require(headers.get(b"content-length", b"0") == b"0", f"204 length: {headers}")
        elif peer is None or b"content-length" in headers:
            require(headers.get(b"content-length") == str(len(payload)).encode(),
                    f"Content-Length does not describe {len(payload)} received bytes: {headers}")
        if encoded:
            require(headers.get(b"content-encoding") == b"gzip", f"Expected gzip: {headers}")
            vary = {part.strip() for part in headers.get(b"vary", b"").lower().split(b",")}
            require(b"accept-encoding" in vary, f"Missing Accept-Encoding in Vary: {headers}")
            require(gzip.decompress(payload) == expected_body, "Gzip bytes do not decode to the response")
        else:
            require(b"content-encoding" not in headers and payload == expected_body,
                    f"Identity fallback or no-body mismatch on {path!r}, encoding {encoding!r}: {headers}")


class Consumer:
    def __init__(self, process):
        self.process = process

    async def read(self):
        marker = (await asyncio.wait_for(self.process.stdout.readline(), 8)).decode().strip()
        require(marker, f"Installed consumer stopped at {self.process.returncode}")
        return marker

    async def expect(self, expected):
        marker = await self.read()
        require(marker == expected, f"Expected {expected!r}, got {marker!r}")

    async def send(self, command):
        self.process.stdin.write(command.encode() + b"\n")
        await self.process.stdin.drain()

    async def pressure(self, command, *, expect_write=False):
        await self.send(command)
        marker = await self.read()
        count = re.fullmatch(r"PRESSURE writes=(\d+) limit=(\d+)", marker)
        require(count is not None, f"Missing typed pressure: {marker!r}")
        require(int(count[2]) == SLOW_DATA_LIMIT and (not expect_write or int(count[1]) > 0), marker)
        print(marker, flush=True)


async def exercise(args):
    port = 0
    if args.protocol == "1.1":
        with socket.socket() as probe:
            probe.bind(("127.0.0.1", 0))
            port = probe.getsockname()[1]
    process = await asyncio.create_subprocess_exec(
        str(args.consumer), args.mode, args.protocol, str(args.certificate), str(args.key), str(port),
        stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
    )
    control = Consumer(process)
    try:
        ready = re.fullmatch(r"READY (\d+) ABI 10", await control.read())
        require(ready is not None, "Missing installed ABI 10 readiness")
        port = int(ready[1])
        if args.protocol == "1.1":
            if args.mode == "gzip":
                await gzip_fallback(None, port)
            else:
                await slow_sse_h1(port, control, args.mode == "write-timeout")
        else:
            if args.protocol == "3":
                from lifecycle_http3_peer import h3_peer

                connection = h3_peer(port, args.ca, 1024)
            else:
                from lifecycle_interactions_wire import h2_peer

                connection = h2_peer(port, args.ca)
            async with connection as peer:
                if args.mode == "gzip":
                    await gzip_fallback(peer, port)
                else:
                    await slow_sse_multiplexed(peer, args.protocol, port, control)

        terminals = {"gzip": 5, "sse-pressure": 3, "write-timeout": 4}
        await control.expect(f"TERMINALS {terminals[args.mode]}")
        await control.send("close")
        await control.expect("REOPENED")
        require(await asyncio.wait_for(process.wait(), 6) == 0, "Consumer cleanup failed")
        print(f"PASS installed ABI 10 HTTP/{args.protocol} {args.mode}", flush=True)
    finally:
        if process.returncode is None:
            process.kill()
            await process.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("consumer", type=Path)
    parser.add_argument("mode", choices=("gzip", "sse-pressure", "write-timeout"))
    parser.add_argument("protocol", choices=("1.1", "2", "3"))
    for option in ("certificate", "key", "ca"):
        parser.add_argument(option, type=Path)
    asyncio.run(asyncio.wait_for(exercise(parser.parse_args()), 36))


if __name__ == "__main__":
    main()
