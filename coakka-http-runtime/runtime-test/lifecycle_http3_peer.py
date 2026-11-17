"""Controlled standalone HTTP/3 peer for native lifecycle tests."""

import asyncio
from contextlib import asynccontextmanager
from dataclasses import dataclass, field

from aioquic import tls
from aioquic.asyncio import QuicConnectionProtocol, connect
from aioquic.h3.connection import H3_ALPN, H3Connection
from aioquic.h3.events import DataReceived, HeadersReceived
from aioquic.quic.configuration import QuicConfiguration
from aioquic.quic.events import ConnectionTerminated, StreamReset


@dataclass
class Response:
    status: bytes = b""
    headers: dict[bytes, bytes] = field(default_factory=dict)
    body: bytearray = field(default_factory=bytearray)
    ended: bool = False
    reset: bool = False


class Http3Peer(QuicConnectionProtocol):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.http = H3Connection(self._quic)
        self.responses = {}
        self.changed = asyncio.Event()
        self.closed = None
        self.held_stream = None
        self.hold_acks = False
        self.deferred_ack_space = None
        write_ack = self._quic._write_ack_frame

        def delay_application_ack(builder, space, now):
            if (self.hold_acks and
                    space is self._quic._spaces[tls.Epoch.ONE_RTT]):
                self.deferred_ack_space = space
                space.ack_at = None
                return
            write_ack(builder, space, now)

        self._quic._write_ack_frame = delay_application_ack
        write_limits = self._quic._write_stream_limits

        def limit_one_response(builder, space, stream):
            # Keep the negotiated window for one response. All sibling streams
            # and transport ACKs continue using normal client flow control.
            if stream.stream_id != self.held_stream:
                write_limits(builder, space, stream)

        self._quic._write_stream_limits = limit_one_response

    @property
    def request_credit(self):
        return self._quic._remote_max_streams_bidi

    def datagram_received(self, data, addr):
        super().datagram_received(data, addr)
        self.changed.set()  # MAX_STREAMS has no application event.

    def resume_acks(self):
        self.hold_acks = False
        if self.deferred_ack_space is not None:
            self.deferred_ack_space.ack_at = 0.0
            self.deferred_ack_space = None
        self.transmit()

    def quic_event_received(self, event):
        if isinstance(event, ConnectionTerminated):
            self.closed = event
        elif isinstance(event, StreamReset) and event.stream_id in self.responses:
            self.responses[event.stream_id].reset = True
        for message in self.http.handle_event(event):
            response = self.responses[message.stream_id]
            if isinstance(message, HeadersReceived):
                response.headers = dict(message.headers)
                response.status = response.headers[b":status"]
            elif isinstance(message, DataReceived):
                response.body.extend(message.data)
            response.ended = message.stream_ended
        self.changed.set()

    async def until(self, predicate):
        async def wait():
            while not predicate():
                if self.closed is not None:
                    raise RuntimeError(f"QUIC closed before the expected event: {self.closed}")
                self.changed.clear()
                await self.changed.wait()

        await asyncio.wait_for(wait(), 6)

    async def request(self, path, method=b"GET", body_size=32, extra_headers=()):
        stream = self._quic.get_next_available_stream_id()
        headers = [(b":method", method), (b":scheme", b"https"),
                   (b":authority", b"localhost"), (b":path", path)]
        if method == b"POST":
            headers.append((b"content-length", str(body_size).encode()))
        headers.extend(extra_headers)
        self.responses[stream] = Response()
        if path in (b"/download", b"/stream"):
            self.held_stream = stream
        self.http.send_headers(stream, headers, end_stream=method != b"POST")
        self.transmit()
        return stream

    async def send_data(self, stream, data=b"held", *, end_stream=False):
        self.http.send_data(stream, data, end_stream=end_stream)
        self.transmit()

    async def reset(self, stream):
        self._quic.reset_stream(stream, 0x10C)  # H3_REQUEST_CANCELLED
        self.transmit()

    async def resume(self, stream):
        if stream != self.held_stream:
            raise RuntimeError("Resuming the wrong stream")
        self.held_stream = None
        self.transmit()

    async def verify(self, stream, body):
        response = self.responses[stream]
        await self.until(lambda: response.ended)
        if response.status != b"200" or response.body != body:
            raise RuntimeError(
                f"Unexpected response on {stream}: {response.status!r}, bytes={len(response.body)}"
            )


@asynccontextmanager
async def h3_peer(port, certificate, window):
    config = QuicConfiguration(is_client=True, alpn_protocols=H3_ALPN,
                               server_name="localhost", cafile=str(certificate),
                               max_stream_data=window)
    async with connect("127.0.0.1", port, configuration=config,
                       create_protocol=Http3Peer) as peer:
        await asyncio.wait_for(peer.ping(), 4)
        yield peer
