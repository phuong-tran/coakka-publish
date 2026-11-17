"""Independent TLS/HTTP/3 peer for the installed host ABI lifecycle fixture."""
import argparse
import asyncio
import ssl
import socket
import struct

from aioquic.asyncio import QuicConnectionProtocol, connect
from aioquic.buffer import Buffer
from aioquic.h3.connection import FrameType, H3_ALPN, H3Connection, Setting
from aioquic.h3.events import DataReceived, HeadersReceived
from aioquic.quic.configuration import QuicConfiguration
from aioquic.quic.events import (
    ConnectionTerminated,
    HandshakeCompleted,
    StreamReset,
)
from aioquic.quic.logger import QuicLogger

PENDING_STREAM_WINDOW = 4096
PENDING_WEBSOCKET_PAYLOAD = 16 * 1024
RESET_SIGNALS = {
    "reset-stream": "RESET_STREAM",
    "reset-stop": "STOP_SENDING",
    "reset-fin": "FIN",
    "reset-pre-accept": "RESET_STREAM",
    "reset-pre-accept-stop": "STOP_SENDING",
    "reset-pre-accept-fin": "FIN",
    "reset-pending-write": "STOP_SENDING",
    "reset-pending-write-stream": "RESET_STREAM",
    "reset-pending-write-fin": "FIN",
}


def frame(opcode, payload):
    assert len(payload) < 126
    mask = b"abcd"
    return bytes((0x80 | opcode, 0x80 | len(payload))) + mask + bytes(
        byte ^ mask[index % 4] for index, byte in enumerate(payload)
    )


class ObservedH3(H3Connection):
    def __init__(self, quic):
        super().__init__(quic)
        self.goaway = []

    def _handle_control_frame(self, frame_type, frame_data):
        super()._handle_control_frame(frame_type, frame_data)
        if frame_type == FrameType.GOAWAY:
            buffer = Buffer(data=frame_data)
            self.goaway.append(buffer.pull_uint_var())
            assert buffer.eof()


class Peer(QuicConnectionProtocol):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.http = None
        self.events = {}
        self.resets = {}
        self.alpn = None
        self.held_credit_stream = None
        write_limits = self._quic._write_stream_limits

        def hold_one_window(builder, space, stream):
            # Keep A's advertised receive window fixed. B and connection
            # credit and transport ACKs continue on the normal path.
            if stream.stream_id != self.held_credit_stream:
                write_limits(builder, space, stream)

        self._quic._write_stream_limits = hold_one_window

    def quic_event_received(self, event):
        if isinstance(event, HandshakeCompleted):
            self.alpn = event.alpn_protocol
            self.http = ObservedH3(self._quic)
        if isinstance(event, ConnectionTerminated) and self.alpn is None:
            print(f"Handshake terminated: {event!r}", flush=True)
        if isinstance(event, StreamReset):
            self.resets.setdefault(event.stream_id, asyncio.Queue()).put_nowait(
                event.error_code
            )
        if self.http is None:
            return
        for received in self.http.handle_event(event):
            if getattr(self, "trace", False):
                print(f"HTTP event {received!r}", flush=True)
            self.events.setdefault(received.stream_id, asyncio.Queue()).put_nowait(received)

    def send_headers(self, path, websocket=True, *, hold_credit=False):
        stream = self._quic.get_next_available_stream_id()
        properties = [
            (b":method", b"CONNECT" if websocket else b"GET"),
            (b":scheme", b"https"),
            (b":authority", b"localhost"),
            (b":path", path.encode("ascii")),
        ]
        if websocket:
            properties += [
                (b":protocol", b"websocket"),
                (b"sec-websocket-version", b"13"),
                (b"sec-websocket-protocol", b"echo"),
            ]
        if hold_credit:
            self.held_credit_stream = stream
        self.http.send_headers(stream, properties, end_stream=not websocket)
        self.transmit()
        return stream

    async def headers(self, path, websocket=True, *, hold_credit=False):
        stream = self.send_headers(path, websocket, hold_credit=hold_credit)
        observed = await self.next_event(stream)
        assert isinstance(observed, HeadersReceived), observed
        fields = dict(observed.headers)
        assert fields[b":status"] == b"200", fields
        if websocket:
            assert fields[b"sec-websocket-protocol"] == b"echo", fields
            assert not observed.stream_ended
            assert self.http.received_settings[Setting.ENABLE_CONNECT_PROTOCOL] == 1
        return stream

    async def next_event(self, stream):
        return await asyncio.wait_for(
            self.events.setdefault(stream, asyncio.Queue()).get(), 4
        )

    def send(self, stream, opcode, payload, *, end=False):
        self.http.send_data(stream, frame(opcode, payload), end_stream=end)
        self.transmit()

    async def body(self, stream, size, *, fin=False):
        body = bytearray()
        while True:
            message = await self.next_event(stream)
            assert isinstance(message, DataReceived), message
            body.extend(message.data)
            assert len(body) <= size, body
            if message.stream_ended:
                assert fin and len(body) == size, body
                break
            if not fin and len(body) == size:
                break
        return bytes(body)


def transport_events(logger):
    return logger.to_dict()["traces"][0]["events"]


def cid_sequences(logger, event_name, frame_type):
    return {
        frame["sequence_number"]
        for event in transport_events(logger)
        if event["name"] == event_name
        for frame in event["data"].get("frames", ())
        if frame["frame_type"] == frame_type
    }


def check_default_allowance(logger):
    allowances = {
        event["data"].get("active_connection_id_limit")
        for event in transport_events(logger)
        if event["name"] == "transport:parameters_set"
        and event["data"].get("owner") == "local"
    }
    assert allowances == {8}, allowances


def configuration(cert, logger, *, pending_write=False):
    return QuicConfiguration(
        is_client=True, alpn_protocols=H3_ALPN, server_name="localhost",
        verify_mode=ssl.CERT_REQUIRED, cafile=cert, idle_timeout=5,
        supported_versions=[1], quic_logger=logger,
        **({"max_stream_data": PENDING_STREAM_WINDOW} if pending_write else {}),
    )


async def wait_for_http(peer):
    assert peer.alpn == "h3", peer.alpn
    async with asyncio.timeout(3):
        while peer.http.received_settings is None:
            await asyncio.sleep(0.002)


def retired_cids(logger):
    return cid_sequences(logger, "transport:packet_sent", "retire_connection_id")


async def check_cid_step(peer, logger, index):
    def issued_cids():
        return cid_sequences(logger, "transport:packet_received", "new_connection_id")

    async with asyncio.timeout(3):
        while index + 3 not in issued_cids():
            await asyncio.sleep(0.002)
    assert issued_cids() == set(range(1, index + 4))
    if index < 7:
        peer.change_connection_id()
        async with asyncio.timeout(3):
            while len(retired_cids(logger)) < index + 1:
                await asyncio.sleep(0.002)


def check_cid_result(logger, fixed_cid):
    assert len(retired_cids(logger)) == (0 if fixed_cid else 7)
    if fixed_cid:
        print("HTTP/3 fixed CID: 8 requests on one connection", flush=True)
        return
    print("HTTP/3 default CID allowance=8, initial plus 3 issued, "
          "7 retired and replaced on one connection", flush=True)


async def check_ordinary(peer, logger, *, fixed_cid, websocket_enabled):
    if websocket_enabled:
        assert peer.http.received_settings[Setting.ENABLE_CONNECT_PROTOCOL] == 1
    else:
        assert Setting.ENABLE_CONNECT_PROTOCOL not in peer.http.received_settings
    for index in range(8):
        stream = await peer.headers("/plain", False)
        assert stream == 4 * index, stream
        assert await peer.body(stream, 8, fin=True) == b"ordinary"
        if not fixed_cid:
            await check_cid_step(peer, logger, index)
    check_cid_result(logger, fixed_cid)


async def read_line(process):
    line = (await asyncio.wait_for(process.stdout.readline(), 7)).decode().strip()
    assert line, f"fixture closed stdout, status={process.returncode}"
    print(line, flush=True)
    return line


async def exercise(args, mode):
    flags = ["--io-uring"] if args.io_uring else []
    process = await asyncio.create_subprocess_exec(
        args.fixture, args.cert, args.key, mode, *flags,
        stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
    )
    try:
        backend = await read_line(process)
        assert backend.startswith("IO_URING "), backend
        server = await read_line(process)
        assert server.startswith("PORT="), server
        port = int(server.split("=")[1])
        logger = QuicLogger()
        config = configuration(args.ca or args.cert, logger,
                               pending_write=mode.startswith("reset-pending-write"))
        async with connect(
            "127.0.0.1", port, configuration=config, create_protocol=Peer,
        ) as peer:
            peer.trace = args.trace
            await wait_for_http(peer)
            check_default_allowance(logger)
            if mode in {"ordinary", "ordinary-with-ws"}:
                await check_ordinary(
                    peer, logger, fixed_cid=args.fixed_cid,
                    websocket_enabled=mode == "ordinary-with-ws",
                )
                process.stdin.write(b"WIRE_CLOSED\n")
                await process.stdin.drain()
            else:
                assert peer.http.received_settings[Setting.ENABLE_CONNECT_PROTOCOL] == 1
                await exercise_websocket(peer, process, mode)
            assert await read_line(process) == "PASS PORT_REUSED"
            assert await asyncio.wait_for(process.wait(), 5) == 0
        print(f"PASS {mode}", flush=True)
    finally:
        if args.trace and "logger" in locals():
            for event in transport_events(logger):
                if event["name"] in {
                    "transport:packet_sent", "transport:packet_received",
                    "transport:packet_dropped", "transport:datagrams_sent",
                }:
                    types = [frame["frame_type"] for frame in event["data"].get("frames", ())]
                    print(event["name"], types)
        if process.returncode is None:
            process.kill()
            await process.wait()


async def check_live_sibling(peer, process, sibling):
    peer.send(sibling, 1, b"still-live")
    assert await peer.body(sibling, 12) == b"\x81\x0astill-live"
    ordinary = await peer.headers("/plain", False)
    assert ordinary == 8
    assert await peer.body(ordinary, 8, fin=True) == b"ordinary"
    peer.send(sibling, 8, struct.pack("!H", 1000), end=True)
    assert await peer.body(sibling, 4, fin=True) == b"\x88\x02\x03\xe8"
    process.stdin.write(b"WIRE_CLOSED\n")
    await process.stdin.drain()


async def wait_for_pending_write(peer, stream):
    partial = bytearray()
    receiver = peer._quic._streams[stream].receiver
    # The sender cannot finish this accepted frame within A's fixed
    # window. Read until that window is spent, then stop the stream.
    while receiver.highest_offset < PENDING_STREAM_WINDOW or not partial:
        message = await peer.next_event(stream)
        assert isinstance(message, DataReceived) and not message.stream_ended, message
        partial.extend(message.data)
    frame_head = b"\x82\x7e" + struct.pack("!H", PENDING_WEBSOCKET_PAYLOAD)
    assert partial.startswith(frame_head), partial[:4]
    assert 4 < len(partial) < len(frame_head) + PENDING_WEBSOCKET_PAYLOAD
    assert receiver.highest_offset == PENDING_STREAM_WINDOW
    print(f"WRITE_PENDING stream={stream} credit={receiver.highest_offset} "
          f"frame_bytes={len(partial)}", flush=True)


def send_reset_signal(peer, stream, mode):
    signal = RESET_SIGNALS[mode]
    if signal == "RESET_STREAM":
        peer._quic.reset_stream(stream, 0x10C)
    elif signal == "STOP_SENDING":
        peer._quic.stop_stream(stream, 0x10C)
    else:
        # A bare transport FIN has no WebSocket CLOSE or HTTP/3 DATA frame.
        peer._quic.send_stream_data(stream, b"", end_stream=True)
    peer.transmit()
    print(f"PEER_CLOSE mode={mode} stream={stream} signal={signal}", flush=True)


async def exercise_websocket(peer, process, mode):
    if mode.startswith("reset-pre-accept"):
        b = await peer.headers("/b")
        a = peer.send_headers("/a")
        assert (b, a) == (0, 4)
        assert await read_line(process) == "PRE_ACCEPT"
        send_reset_signal(peer, a, mode)
        assert await read_line(process) == "A_CLOSED"
        await check_live_sibling(peer, process, b)
        assert a not in peer.events or peer.events[a].empty()
        return

    pending_write = mode.startswith("reset-pending-write")
    a = await peer.headers("/a", hold_credit=pending_write)
    if mode in RESET_SIGNALS:
        b = await peer.headers("/b")
        assert (a, b) == (0, 4)
        if pending_write:
            assert await read_line(process) == "WRITE_ACCEPTED"
            await wait_for_pending_write(peer, a)
        else:
            peer.send(a, 1, b"held")
            assert await read_line(process) == "HELD"
        send_reset_signal(peer, a, mode)
        assert await read_line(process) == "A_CLOSED"
        await check_live_sibling(peer, process, b)
    else:
        peer.send(a, 1, b"held")
        expected = "HELD" if mode == "shutdown" else "DRAIN_PENDING"
        assert await read_line(process) == expected
        if mode == "complete":
            async with asyncio.timeout(1.2):
                while len(peer.http.goaway) < 2:
                    await asyncio.sleep(0.002)
            assert peer.http.goaway == [(1 << 62) - 4, 4], peer.http.goaway
            later = peer._quic.get_next_available_stream_id()
            assert later == 4
            peer.http.send_headers(later, [
                (b":method", b"GET"), (b":scheme", b"https"),
                (b":authority", b"localhost"), (b":path", b"/plain"),
            ], end_stream=True)
            peer.transmit()
            reset_code = await asyncio.wait_for(
                peer.resets.setdefault(later, asyncio.Queue()).get(), 1
            )
            assert reset_code == 0x10B
            peer.send(a, 8, struct.pack("!H", 1000), end=True)
            assert await peer.body(a, 4, fin=True) == b"\x88\x02\x03\xe8"
        elif mode in {"aborted", "shutdown"}:
            process.stdin.write(b"STOP\n")
            await process.stdin.drain()


def fixed_port():
    for _ in range(8):
        with socket.socket() as tcp, socket.socket(type=socket.SOCK_DGRAM) as udp:
            tcp.bind(("127.0.0.1", 0))
            port = tcp.getsockname()[1]
            try:
                udp.bind(("127.0.0.1", port))
            except OSError:
                continue
            return port
    raise RuntimeError("could not select a fixed test port for TCP and UDP")


async def run_tcp(*command):
    process = await asyncio.create_subprocess_exec(
        *command, stdout=asyncio.subprocess.PIPE,
    )
    try:
        output, _ = await asyncio.wait_for(process.communicate(), 8)
        assert process.returncode == 0, (command[0], output)
        return output.strip()
    finally:
        if process.returncode is None:
            process.kill()
            await process.wait()


async def check_v1_wave(peer, args, port, curl, index):
    async with asyncio.TaskGroup() as traffic:
        http1 = traffic.create_task(run_tcp(*curl, "--http1.1"))
        if args.python_http2:
            from http2_io_backend_peer import get
            http2 = traffic.create_task(get(port, args.cert, "/same-port"))
        else:
            http2 = traffic.create_task(run_tcp(*curl, "--http2"))
        stream = await peer.headers("/same-port", False)
        assert stream == 4 * index, stream
        assert await peer.body(stream, 12, fin=True) == b"same-port-ok"
    assert http1.result() == b"same-port-ok\n1.1"
    if args.python_http2:
        assert http2.result() == ("200", b"same-port-ok")
    else:
        assert http2.result() == b"same-port-ok\n2"


async def check_v1(args):
    port = fixed_port()
    mode = "--cid-waves" if args.v1_cid_waves else "--graceful"
    process = await asyncio.create_subprocess_exec(
        args.fixture, args.cert, args.key, mode, str(port),
        stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
    )
    try:
        assert await read_line(process) == f"PORT={port}"
        logger = QuicLogger()
        async with connect(
            "127.0.0.1", port, configuration=configuration(args.cert, logger),
            create_protocol=Peer,
        ) as peer:
            await wait_for_http(peer)
            check_default_allowance(logger)
            assert Setting.ENABLE_CONNECT_PROTOCOL not in peer.http.received_settings
            curl = (
                args.curl, "--noproxy", "*", "--silent", "--show-error", "--fail",
                "--cacert", args.cert, "--resolve", f"localhost:{port}:127.0.0.1",
                "--write-out", "\n%{http_version}", f"https://localhost:{port}/same-port",
            )
            for index in range(8 if args.v1_cid_waves else 1):
                await check_v1_wave(peer, args, port, curl, index)
                if args.v1_cid_waves and not args.fixed_cid:
                    await check_cid_step(peer, logger, index)
                process.stdin.write(b"received\n")
                await process.stdin.drain()
                if args.v1_cid_waves:
                    assert await read_line(process) == f"TRAFFIC_WAVE={index + 1}; TERMINALS=3"
            if args.v1_cid_waves:
                check_cid_result(logger, args.fixed_cid)
                assert await read_line(process) == "TRAFFIC_SOAK=24; STOPPED"
            else:
                assert await read_line(process) == "PUBLIC_V1=DRAIN_COMPLETE; HELD_LEASE=RETRY"
        assert await asyncio.wait_for(process.wait(), 5) == 0
        print("PASS V1 HTTP/1.1 + HTTP/2 + default HTTP/3 on the same fixed port")
    finally:
        if process.returncode is None:
            process.kill()
            await process.wait()


async def main(args):
    if args.v1 or args.v1_cid_waves:
        await check_v1(args)
    else:
        for mode in args.modes:
            await exercise(args, mode)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", action="store_true")
    parser.add_argument("--curl", default="curl")
    parser.add_argument("--ca", help="CA PEM used to verify the loopback listener")
    parser.add_argument("--python-http2", action="store_true",
                        help="use the verified Python HTTP/2 client for V1")
    parser.add_argument("--io-uring", action="store_true")
    parser.add_argument("--fixed-cid", action="store_true",
                        help="keep the original CID in the ordinary case")
    parser.add_argument("--v1", action="store_true", help="run the V1 same-port test")
    parser.add_argument("--v1-cid-waves", action="store_true",
                        help="run 8 same-port waves on one HTTP/3 connection")
    parser.add_argument("fixture")
    parser.add_argument("cert")
    parser.add_argument("key")
    parser.add_argument(
        "modes", nargs="*", default=[
            "ordinary", *RESET_SIGNALS,
            "complete", "expired", "aborted", "shutdown",
        ],
    )
    asyncio.run(main(parser.parse_args()))
