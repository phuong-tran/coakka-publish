"""Exercise upload reset, active file drain and slow response on local SDK consumers."""

import argparse
import asyncio
from contextlib import asynccontextmanager
from dataclasses import dataclass, field
from pathlib import Path
import socket
import ssl
import tempfile

from h2.config import H2Configuration
from h2.connection import H2Connection
from h2.errors import ErrorCodes
from h2.events import DataReceived, ResponseReceived, SettingsAcknowledged, StreamEnded, StreamReset
from h2.settings import SettingCodes
from h2.frame_buffer import FrameBuffer
from hyperframe.frame import GoAwayFrame


MULTIPART = (
    b"--local-form\r\nContent-Disposition: form-data; name=\"file\"; filename=\"a.bin\"\r\n"
    b"Content-Type: application/octet-stream\r\n\r\na\0b\r\n--local-form--\r\n"
)
MULTIPART_HEADERS = [(b"content-type", b"multipart/form-data; boundary=local-form")]


WINDOW = 1024
FILE = bytes(range(256)) * 192


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


@dataclass
class Response:
    status: bytes = b""
    body: bytearray = field(default_factory=bytearray)
    ended: bool = False
    reset: bool = False


class Http2Peer:
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.http = H2Connection(config=H2Configuration(client_side=True))
        self.responses = {}
        self.settings_pending = 2
        self.frames = FrameBuffer()
        self.frames.max_frame_size = 16384
        self.goaway = None

    async def flush(self):
        self.writer.write(self.http.data_to_send())
        await self.writer.drain()

    async def receive(self):
        block = await self.reader.read(65536)
        require(block, "HTTP/2 peer closed the connection")
        self.frames.add_data(block)
        for frame in self.frames:
            if isinstance(frame, GoAwayFrame):
                # hyper-h2 closes its whole connection state on GOAWAY.
                # An admitted stream still sends DATA after a graceful GOAWAY.
                require(frame.error_code == 0, f"Unexpected GOAWAY error: {frame.error_code}")
                require(self.goaway is None or frame.last_stream_id <= self.goaway,
                        "GOAWAY increased the stream cutoff")
                self.goaway = frame.last_stream_id
                continue
            self.accept(frame)
        await self.flush()

    def accept(self, frame):
        for event in self.http.receive_data(frame.serialize()):
            if isinstance(event, SettingsAcknowledged):
                self.settings_pending -= 1
            if isinstance(event, (ResponseReceived, DataReceived, StreamEnded, StreamReset)):
                response = self.responses[event.stream_id]
                if isinstance(event, ResponseReceived):
                    response.status = dict(event.headers)[b":status"]
                elif isinstance(event, DataReceived):
                    response.body.extend(event.data)
                elif isinstance(event, StreamEnded):
                    response.ended = True
                else:
                    response.reset = True

    async def until(self, predicate):
        async def wait():
            while not predicate():
                await self.receive()

        await asyncio.wait_for(wait(), 6)

    async def start(self):
        self.http.initiate_connection()
        self.http.update_settings({SettingCodes.INITIAL_WINDOW_SIZE: WINDOW})
        await self.flush()
        await self.until(lambda: self.settings_pending == 0)

    async def request(self, path, method=b"GET", body_size=32, extra_headers=()):
        stream = self.http.get_next_available_stream_id()
        headers = [(b":method", method), (b":scheme", b"https"),
                   (b":authority", b"localhost"), (b":path", path)]
        if method == b"POST":
            headers.append((b"content-length", str(body_size).encode()))
        headers.extend(extra_headers)
        self.responses[stream] = Response()
        self.http.send_headers(stream, headers, end_stream=method != b"POST")
        await self.flush()
        return stream

    async def send_data(self, stream, data=b"held", *, end_stream=False):
        self.http.send_data(stream, data, end_stream=end_stream)
        await self.flush()

    async def reset(self, stream):
        self.http.reset_stream(stream, ErrorCodes.CANCEL)
        await self.flush()

    async def resume(self, stream):
        self.http.increment_flow_control_window(len(FILE), stream)
        await self.flush()

    async def verify(self, stream, body):
        response = self.responses[stream]
        await self.until(lambda: response.ended)
        require(response.status == b"200" and response.body == body,
                f"Unexpected response on {stream}: {response.status!r}, bytes={len(response.body)}")


@asynccontextmanager
async def h2_peer(port, certificate):
    context = ssl.create_default_context(cafile=str(certificate))
    context.set_alpn_protocols(["h2"])
    reader, writer = await asyncio.wait_for(asyncio.open_connection(
        "127.0.0.1", port, ssl=context, server_hostname="localhost"), 5)
    try:
        require(writer.get_extra_info("ssl_object").selected_alpn_protocol() == "h2",
                "TLS did not select HTTP/2")
        peer = Http2Peer(reader, writer)
        await peer.start()
        yield peer
    finally:
        writer.close()


async def mixed_tcp_control(port, certificate):
    context = ssl.create_default_context(cafile=str(certificate))
    context.set_alpn_protocols(["http/1.1"])
    reader, writer = await asyncio.wait_for(asyncio.open_connection(
        "127.0.0.1", port, ssl=context, server_hostname="localhost"), 5)
    try:
        require(writer.get_extra_info("ssl_object").selected_alpn_protocol() == "http/1.1",
                "TLS did not select HTTP/1.1")
        writer.write(b"GET /sibling HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
        await writer.drain()
        head = await asyncio.wait_for(reader.readuntil(b"\r\n\r\n"), 5)
        require(head.startswith(b"HTTP/1.1 200 "), f"Unexpected HTTP/1.1 head: {head!r}")
        require(await asyncio.wait_for(reader.readexactly(6), 5) == b"tcp-ok",
                "Mixed listener HTTP/1.1 body mismatch")
    finally:
        writer.close()
    async with h2_peer(port, certificate) as peer:
        stream = await peer.request(b"/sibling")
        await peer.verify(stream, b"tcp-ok")
        try:
            await asyncio.wait_for(writer.wait_closed(), 5)
        except ConnectionAbortedError as error:
            # The Windows proactor may report its local close after the full HTTP/1.1 reply.
            if getattr(error, "winerror", None) != 10053:
                raise


async def multipart_http1(port, expect):
    header = (
        b"POST /upload HTTP/1.1\r\nHost: localhost\r\n"
        b"Content-Type: multipart/form-data; boundary=local-form\r\n"
        + f"Content-Length: {len(MULTIPART)}\r\nConnection: close\r\n\r\n".encode()
    )

    async def open_post():
        reader, writer = await asyncio.wait_for(asyncio.open_connection("127.0.0.1", port), 5)
        writer.write(header)
        await writer.drain()
        return reader, writer

    _, writer = await open_post()
    try:
        await expect("UPLOAD")
        writer.write(MULTIPART[:31])
        await writer.drain()
        await expect("RESET")
    finally:
        writer.close()
        await asyncio.wait_for(writer.wait_closed(), 4)
    await expect("RECOVERED")

    reader, writer = await open_post()
    try:
        for i in range(0, len(MULTIPART), 11):
            writer.write(MULTIPART[i:i + 11])
            await writer.drain()
        response = await asyncio.wait_for(reader.read(), 5)
        head, _, body = response.partition(b"\r\n\r\n")
        require(head.startswith(b"HTTP/1.1 200 ") and body == b"upload-recovered",
                f"Multipart recovery returned {response!r}")
    finally:
        writer.close()
        await asyncio.wait_for(writer.wait_closed(), 4)


async def exercise(options, mode):
    with tempfile.TemporaryDirectory(prefix="native-lifecycle-", dir=options.scratch) as root:
        (Path(root) / "asset.bin").write_bytes(FILE)
        fixed_port = []
        if options.protocol == "1.1":
            with socket.socket() as probe:
                probe.bind(("127.0.0.1", 0))
                fixed_port.append(str(probe.getsockname()[1]))
        owner = await asyncio.create_subprocess_exec(
            str(options.consumer), mode, options.protocol, str(options.certificate),
            str(options.key), root, *fixed_port, stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE)

        async def read_marker():
            return (await asyncio.wait_for(owner.stdout.readline(), 7)).decode().strip()

        async def expect(expected):
            observed = await read_marker()
            require(observed == expected, f"Expected {expected}, got {observed!r}")

        async def send(command):
            owner.stdin.write(command.encode() + b"\n")
            await owner.stdin.drain()

        try:
            ready = await read_marker()
            require(ready.startswith("READY "), f"Missing listener state: {ready!r}")
            port = int(ready.split()[1])
            if options.protocol == "1.1":
                await multipart_http1(port, expect)
            else:
                if options.protocol == "3":
                    from lifecycle_http3_peer import h3_peer

                    connection = h3_peer(port, options.ca or options.certificate, WINDOW)
                else:
                    connection = h2_peer(port, options.ca or options.certificate)
                async with connection as peer:
                    if mode.startswith("reset-data-followup"):
                        shared = mode.endswith("-shared")
                        require(peer.request_credit == 2,
                                f"Expected two initial stream credits, got {peer.request_credit}")
                        upload = await peer.request(b"/upload", b"POST")
                        if shared:
                            await peer.send_data(upload)
                        else:
                            await expect("UPLOAD")
                        sibling = await peer.request(b"/sibling")
                        await expect("SIBLING")
                        if not shared:
                            await peer.send_data(upload)
                        await expect("RESET")
                        peer.hold_acks = True
                        await peer.reset(upload)
                        await send("reset-sent")
                        await peer.verify(sibling, b"sibling-ok")
                        await expect("RECOVERED")
                        followup = await peer.request(b"/sibling")
                        await send("early-sent")
                        await asyncio.sleep(0.05)
                        require(peer.request_credit == 2 and not peer.responses[followup].reset,
                                "Credit escaped while B's final response was unacknowledged")
                        peer.resume_acks()
                        await expect("FOLLOW-UP")
                        await peer.until(lambda: peer.request_credit >= 4)
                        require(peer.request_credit == 4,
                                f"Expected one grant per retired stream, got {peer.request_credit}")
                        await send("respond")
                        await peer.verify(followup, b"next-ok")
                        if shared:
                            await expect("TCP_READY")
                            await mixed_tcp_control(port, options.ca or options.certificate)
                        await expect("DRAIN_READY")
                        await send("drain")
                        await expect("DRAINED")
                    elif mode.startswith("reset-"):
                        upload = await peer.request(b"/upload", b"POST")
                        await expect("UPLOAD")
                        sibling = await peer.request(b"/sibling")
                        await expect("SIBLING")
                        if mode == "reset-held-data":
                            await peer.send_data(upload)
                        await expect("RESET")
                        require(not peer.responses[sibling].ended, "Sibling closed before reset")
                        await peer.reset(upload)
                        await send("reset-sent")
                        await peer.verify(sibling, b"sibling-ok")
                        await expect("RECOVERED")
                    elif mode == "multipart-reset":
                        upload = await peer.request(b"/upload", b"POST", len(MULTIPART), MULTIPART_HEADERS)
                        await expect("UPLOAD")
                        await peer.send_data(upload, MULTIPART[:31])
                        await expect("RESET")
                        await peer.reset(upload)
                        await expect("RECOVERED")
                        followup = await peer.request(b"/upload", b"POST", len(MULTIPART), MULTIPART_HEADERS)
                        for i in range(0, len(MULTIPART), 11):
                            await peer.send_data(followup, MULTIPART[i:i + 11],
                                                 end_stream=i + 11 >= len(MULTIPART))
                        await peer.verify(followup, b"upload-recovered")
                    elif mode == "file-drain":
                        stream = await peer.request(b"/download")
                        response = peer.responses[stream]
                        minimum = WINDOW if options.protocol == "2" else WINDOW // 2
                        await peer.until(lambda: len(response.body) >= minimum)
                        prefix = len(response.body)
                        require(prefix <= WINDOW and response.body == FILE[:prefix] and not response.ended,
                                "Expected an in-flight file with a blocked window")
                        await send("drain")
                        await expect("PENDING")
                        await peer.resume(stream)
                        await peer.verify(stream, FILE)
                        await expect("COMPLETE")
                        if options.protocol == "2":
                            require(peer.goaway is not None and stream <= peer.goaway,
                                    "Graceful GOAWAY excluded the accepted download")
                    else:
                        stream = await peer.request(b"/stream")
                        response = peer.responses[stream]
                        await peer.until(lambda: response.status == b"200")
                        await send("write")
                        await expect("WRITING")
                        sibling = await peer.request(b"/sibling")
                        await peer.verify(sibling, b"sibling-ok")
                        await peer.until(lambda: len(response.body) >= WINDOW // 2)
                        require(len(response.body) <= WINDOW and not response.ended,
                                "Client did not hold a partial response")
                        print(f"Sibling completed with {len(response.body)} stalled response bytes",
                              flush=True)
                        if options.diagnostic_wake:
                            await asyncio.sleep(1.2)
                            await asyncio.wait_for(peer.ping(), 3)
                        await expect("WRITE_TIMEOUT")
                        await peer.until(lambda: response.reset)
                        require(0 < len(response.body) <= WINDOW and not response.ended,
                                "Expected partial output followed by reset")
                    if (mode not in ("file-drain", "multipart-reset") and
                            not mode.startswith("reset-data-followup")):
                        followup = await peer.request(b"/sibling")
                        await peer.verify(followup, b"next-ok")
            await send("finish")
            await expect(f"REBOUND {port}")
            require(await asyncio.wait_for(owner.wait(), 5) == 0, "Native consumer failed")
            result = "CONTROL" if options.diagnostic_wake else "PASS"
            print(f"{result} h{options.protocol} {mode}", flush=True)
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
    parser.add_argument("mode", choices=("reset-before-data", "reset-held-data",
                                        "reset-data-followup", "reset-data-followup-shared", "file-drain",
                                        "write-timeout", "multipart-reset"))
    parser.add_argument("--protocol", choices=("1.1", "2", "3"), default="2")
    parser.add_argument("--ca", type=Path, help="Trust root for the fixture certificate")
    parser.add_argument("--diagnostic-wake", action="store_true",
                        help="Send one QUIC PING after the write timeout for diagnosis")
    options = parser.parse_args()
    if options.protocol == "1.1" and options.mode != "multipart-reset":
        parser.error("HTTP/1.1 is available for multipart-reset")
    if options.mode.startswith("reset-data-followup") and options.protocol != "3":
        parser.error("Immediate reset follow-up requires HTTP/3")
    if options.diagnostic_wake and (options.protocol != "3" or options.mode != "write-timeout"):
        parser.error("Diagnostic wake requires HTTP/3 write-timeout")
    asyncio.run(exercise(options, options.mode))


if __name__ == "__main__":
    main()
