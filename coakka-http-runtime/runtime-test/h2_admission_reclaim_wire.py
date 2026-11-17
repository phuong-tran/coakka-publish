"""Drive a quiet HTTP/2 waiter against a consumer of the installed SDK."""

import argparse
from collections import defaultdict
from contextlib import ExitStack
import hashlib
import os
from pathlib import Path
import queue
import socket
import ssl
import subprocess
import threading
import time

from h2.config import H2Configuration
from h2.connection import H2Connection
from h2.errors import ErrorCodes
from h2.events import DataReceived, PingAckReceived, ResponseReceived, StreamEnded, StreamReset


class Fixture:
    def __init__(self, executable, port, active, transport, cert, key, backend, verify_image):
        self.lines = queue.Queue()
        self.host = subprocess.Popen(
            [executable, str(port), str(active), transport, cert, key, backend],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True,
        )
        self.reader = threading.Thread(target=self.read_lines, daemon=True)
        self.reader.start()
        try:
            assert self.read() == f"READY {port} backend={backend}"
            if verify_image and (expected := os.environ.get("COAKKA_EXPECT_LOADED_DLL")):
                self.verify_windows_image(Path(expected))
        except BaseException:
            self.close()
            raise

    def read_lines(self):
        for line in self.host.stdout:
            self.lines.put(line.rstrip())
        self.lines.put(None)

    def read(self):
        line = self.lines.get(timeout=8)
        assert line is not None, f"Host exited with {self.host.poll()}"
        return line

    def command(self, text, expected="OK"):
        assert self.exchange(text) == expected, text

    def exchange(self, text):
        self.host.stdin.write(f"{text}\n")
        self.host.stdin.flush()
        return self.read()

    def verify_windows_image(self, expected):
        powershell = Path(os.environ["SystemRoot"]) / "System32/WindowsPowerShell/v1.0/powershell.exe"
        query = (f"(Get-Process -Id {self.host.pid}).Modules | "
                 f"Where-Object ModuleName -eq '{expected.name}' | ForEach-Object FileName")
        output = subprocess.check_output(
            [powershell, "-NoProfile", "-NonInteractive", "-Command", query],
            text=True, timeout=15,
        )
        actual = {Path(line).resolve(strict=True) for line in output.splitlines() if line.strip()}
        assert actual == {expected.resolve(strict=True)}, actual
        digest = hashlib.sha256(expected.read_bytes()).hexdigest()
        assert digest == os.environ["COAKKA_EXPECT_LOADED_DLL_SHA256"], digest
        print(f"Loaded image {expected} SHA-256 {digest}", flush=True)

    def close(self):
        if self.host.poll() is None:
            self.host.kill()
        self.host.wait(timeout=7)
        self.reader.join(timeout=7)
        self.host.stdin.close()
        self.host.stdout.close()


class Peer:
    def __init__(self, port, tls, root):
        self.transport = socket.create_connection(("127.0.0.1", port), timeout=5)
        self.scheme = "http"
        if tls:
            context = ssl.create_default_context(cafile=str(root))
            context.set_alpn_protocols(["h2"])
            self.transport = context.wrap_socket(self.transport, server_hostname="localhost")
            assert self.transport.selected_alpn_protocol() == "h2"
            self.scheme = "https"
        self.connection = H2Connection(H2Configuration(header_encoding="utf-8"))
        self.pending = bytearray()
        self.status = {}
        self.body = defaultdict(bytearray)
        self.ended = set()
        self.resets = set()
        self.acks = set()
        self.goaway = []
        self.connection.initiate_connection()
        self.flush()

    def flush(self):
        data = self.connection.data_to_send()
        if data:
            self.transport.sendall(data)

    def request(self, path, barrier=None):
        stream = self.connection.get_next_available_stream_id()
        self.connection.send_headers(stream, [
            (":method", "GET"), (":scheme", self.scheme),
            (":authority", "localhost"), (":path", path),
        ], end_stream=True)
        if barrier is not None:
            # Place the PING after HEADERS in the same write. The response
            # establishes peer receipt before A finishes. Then B stays quiet.
            self.connection.ping(barrier)
        self.flush()
        if barrier is not None:
            self.until(lambda: barrier in self.acks)
        return stream

    def reset(self, stream):
        barrier = b"reset-ok"
        self.connection.reset_stream(stream, ErrorCodes.CANCEL)
        self.connection.ping(barrier)
        self.flush()
        self.until(lambda: barrier in self.acks)

    def until(self, predicate):
        deadline = time.monotonic() + 5
        while not predicate():
            remaining = deadline - time.monotonic()
            assert remaining > 0, "Timed out waiting for HTTP/2 wire state"
            self.transport.settimeout(remaining)
            data = self.transport.recv(65536)
            assert data, "HTTP/2 peer closed before expected frame"
            self.pending.extend(data)
            while len(self.pending) >= 9:
                length = int.from_bytes(self.pending[:3], "big")
                frame_size = 9 + length
                if len(self.pending) < frame_size:
                    break
                frame = bytes(self.pending[:frame_size])
                del self.pending[:frame_size]
                if frame[3] == 7:
                    assert length >= 8 and int.from_bytes(frame[13:17], "big") == 0
                    self.goaway.append(int.from_bytes(frame[9:13], "big") & 0x7FFFFFFF)
                    continue
                for event in self.connection.receive_data(frame):
                    if isinstance(event, ResponseReceived):
                        self.status[event.stream_id] = dict(event.headers)[":status"]
                    elif isinstance(event, DataReceived):
                        self.body[event.stream_id].extend(event.data)
                        self.connection.acknowledge_received_data(
                            event.flow_controlled_length, event.stream_id)
                    elif isinstance(event, StreamEnded):
                        self.ended.add(event.stream_id)
                    elif isinstance(event, StreamReset):
                        self.resets.add(event.stream_id)
                    elif isinstance(event, PingAckReceived):
                        self.acks.add(event.ping_data)
            self.flush()

    def expect(self, stream, expected):
        self.until(lambda: stream in self.ended or stream in self.resets)
        assert stream not in self.resets
        assert self.status[stream] == "200" and self.body[stream] == expected


def run(arguments, mode):
    tls = arguments.transport == "tls"
    with socket.socket() as reservation:
        reservation.bind(("127.0.0.1", 0))
        port = reservation.getsockname()[1]
    same_connection = mode == "same"
    active = 2 if same_connection or mode == "spare" else 1
    fixture = Fixture(arguments.executable, port, active, arguments.transport,
                      arguments.cert, arguments.key, arguments.backend, mode == "direct")
    try:
        with ExitStack() as peers:
            def peer():
                value = Peer(port, tls, arguments.root)
                peers.enter_context(value.transport)
                return value

            first = peer()
            first_stream = first.request("/a")
            fixture.command("retain 1" if mode == "late" else "take 1")
            if same_connection:
                filler = peer()
                filler_stream = filler.request("/filler")
                fixture.command("take 4")
            second = first if same_connection else peer()
            second_stream = second.request("/b", barrier=b"b-queued")
            if mode == "many":
                third = peer()
                third_stream = third.request("/c", barrier=b"c-queued")
            if mode == "spare":
                fixture.command("take 2")
            fixture.command("quiet")
            if mode == "stop":
                fixture.command("stop", "STOPPED AND REOPENED")
                assert fixture.host.wait(timeout=7) == 0
                return
            if mode == "cancel":
                second.reset(second_stream)
            if mode == "drain":
                fixture.command("begin")
            fixture.command("answer 1")
            first.expect(first_stream, b"first")
            if mode in {"late", "cancel", "drain"}:
                fixture.command("terminal 1")
                fixture.command("quiet")
            if mode == "late":
                fixture.command("return 1")
            if mode in {"direct", "late", "same", "spare"}:
                if mode != "spare":
                    fixture.command("take 2")
                fixture.command("answer 2")
                second.expect(second_stream, b"second")
                fixture.command("terminal 1")
                fixture.command("terminal 2")
            if mode == "many":
                waiters = {2: (second, second_stream, b"second"),
                           3: (third, third_stream, b"fresh")}
                while waiters:
                    label, route = fixture.exchange("next").split()
                    assert label == "ADMITTED"
                    connection, stream, body = waiters.pop(int(route))
                    fixture.command(f"answer {route}")
                    connection.expect(stream, body)
                for route in (1, 2, 3):
                    fixture.command(f"terminal {route}")
            if mode == "cancel":
                fresh = peer()
                fresh_stream = fresh.request("/c")
                fixture.command("take 3")
                fixture.command("answer 3")
                fresh.expect(fresh_stream, b"fresh")
                fixture.command("terminal 3")
            if same_connection:
                fixture.command("answer 4")
                filler.expect(filler_stream, b"filler")
                fixture.command("terminal 4")
            if mode == "drain":
                first.until(lambda: first.goaway)
                # The parked peer can receive GOAWAY or retire when its
                # transport closes. It must never reach the application.
                assert second_stream not in second.status
            fixture.command("quiet")
        if mode != "drain":
            fixture.command("begin")
        fixture.command("drained")
        fixture.command("stop", "STOPPED AND REOPENED")
        assert fixture.host.wait(timeout=7) == 0
    finally:
        fixture.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    parser.add_argument("cert", type=Path)
    parser.add_argument("key", type=Path)
    parser.add_argument("root", type=Path)
    parser.add_argument("--backend", choices=("default", "uring"), default="default")
    parser.add_argument("--transport", choices=("plain", "tls"), default="tls")
    modes = ("direct", "late", "same", "many", "spare", "cancel", "drain", "stop")
    parser.add_argument("--mode", choices=modes)
    arguments = parser.parse_args()
    if arguments.transport == "plain":
        with socket.socket() as reservation:
            reservation.bind(("127.0.0.1", 0))
            port = reservation.getsockname()[1]
        result = subprocess.run(
            [arguments.executable, str(port), "1", "plain", arguments.cert,
             arguments.key, arguments.backend], text=True, capture_output=True, timeout=8,
        )
        assert result.returncode == 0 and "PLAINTEXT REFUSED" in result.stdout, result
        with socket.socket() as reopened:
            reopened.bind(("127.0.0.1", port))
        print("ABI 10 plaintext startup refusal and TCP port reuse PASS")
        return
    for mode in ((arguments.mode,) if arguments.mode else modes):
        run(arguments, mode)
        print(f"ABI 10 {arguments.transport} {arguments.backend} {mode} PASS", flush=True)


if __name__ == "__main__":
    main()
