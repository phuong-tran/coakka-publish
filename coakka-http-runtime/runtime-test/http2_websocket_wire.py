"""Exercise the installed C host with an independent TLS HTTP/2 peer."""

import argparse
from collections import defaultdict
from pathlib import Path
import queue
import socket
import ssl
import subprocess
import tempfile
import threading
import time

from h2.config import H2Configuration
from h2.connection import H2Connection
from h2.errors import ErrorCodes
from h2.events import (
    DataReceived,
    RemoteSettingsChanged,
    ResponseReceived,
    StreamEnded,
    StreamReset,
)
from h2.settings import SettingCodes


class Host:
    def __init__(self, executable, directory, mode):
        self.process = subprocess.Popen(
            [executable, mode], cwd=directory, stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, text=True,
        )
        self.stdout = queue.Queue()
        self.thread = threading.Thread(target=self.read_lines, daemon=True)
        self.thread.start()
        try:
            port, root = self.read().split()
            self.port = int(port)
            self.trust_root = Path(directory) / root
        except Exception:
            self.close()
            raise

    def read_lines(self):
        for line in self.process.stdout:
            self.stdout.put(line.rstrip())
        self.stdout.put(None)

    def read(self):
        line = self.stdout.get(timeout=7)
        assert line is not None, f"Host exited with {self.process.poll()}"
        return line

    def command(self, text):
        self.process.stdin.write(text + "\n")
        self.process.stdin.flush()
        assert self.read() == "ok", text

    def close(self):
        if self.process.poll() is None:
            self.process.terminate()
        self.process.wait(timeout=7)
        self.thread.join(timeout=7)
        self.process.stdin.close()
        self.process.stdout.close()


class Peer:
    def __init__(self, host, enabled):
        context = ssl.create_default_context(cafile=str(host.trust_root))
        context.set_alpn_protocols(["h2"])
        connection = socket.create_connection(("127.0.0.1", host.port), timeout=5)
        self.socket = context.wrap_socket(connection, server_hostname="localhost")
        assert self.socket.selected_alpn_protocol() == "h2"
        self.connection = H2Connection(H2Configuration(header_encoding="utf-8"))
        self.headers = {}
        self.bodies = defaultdict(bytearray)
        self.ended = set()
        self.resets = {}
        self.goaway = []
        self.setting = None
        self.wire = bytearray()
        self.connection.initiate_connection()
        self.flush()
        self.until(lambda: self.setting is not None)
        assert self.setting == int(enabled)

    def flush(self):
        self.socket.sendall(self.connection.data_to_send())

    def request(self, path, websocket=True):
        stream = self.connection.get_next_available_stream_id()
        headers = [
            (":method", "CONNECT" if websocket else "GET"),
            (":scheme", "https"),
            (":authority", "localhost"),
            (":path", path),
        ]
        if websocket:
            headers.insert(1, (":protocol", "websocket"))
            headers.extend([
                ("sec-websocket-version", "13"),
                ("sec-websocket-protocol", "chat"),
            ])
        self.connection.send_headers(stream, headers, end_stream=not websocket)
        self.flush()
        return stream

    def websocket(self, stream, opcode, payload, end=False):
        # A deterministic mask suffices for this disposable loopback fixture.
        assert len(payload) < 126
        mask = bytes((40, 70, 90, 110))
        data = bytes((0x80 | opcode, 0x80 | len(payload))) + mask
        data += bytes(byte ^ mask[index % 4] for index, byte in enumerate(payload))
        self.connection.send_data(stream, data, end_stream=end)
        self.flush()

    def reset(self, stream):
        self.connection.reset_stream(stream, ErrorCodes.CANCEL)
        self.flush()

    def until(self, ready):
        deadline = time.monotonic() + 5
        while not ready():
            self.socket.settimeout(max(0.001, deadline - time.monotonic()))
            received = self.socket.recv(65536)
            assert received, "Peer closed before expected frame"
            self.wire.extend(received)
            while len(self.wire) >= 9:
                length = int.from_bytes(self.wire[:3], "big")
                if len(self.wire) < 9 + length:
                    break
                frame = bytes(self.wire[:9 + length])
                del self.wire[:9 + length]
                if frame[3] == 7:
                    # Keep processing the old streams after either GOAWAY.
                    # hyper-h2 otherwise treats the connection as fully closed.
                    assert length >= 8 and int.from_bytes(frame[13:17], "big") == 0
                    self.goaway.append(int.from_bytes(frame[9:13], "big") & 0x7FFFFFFF)
                    continue
                for event in self.connection.receive_data(frame):
                    if isinstance(event, RemoteSettingsChanged):
                        change = event.changed_settings.get(SettingCodes.ENABLE_CONNECT_PROTOCOL)
                        self.setting = change.new_value if change else 0
                    elif isinstance(event, ResponseReceived):
                        self.headers[event.stream_id] = dict(event.headers)
                    elif isinstance(event, DataReceived):
                        self.bodies[event.stream_id].extend(event.data)
                        self.connection.acknowledge_received_data(
                            event.flow_controlled_length, event.stream_id,
                        )
                    elif isinstance(event, StreamEnded):
                        self.ended.add(event.stream_id)
                    elif isinstance(event, StreamReset):
                        self.resets[event.stream_id] = event.error_code
            self.flush()

    def open_websocket(self, host, index):
        stream = self.request("/ws")
        host.command(f"accept {index}")
        self.until(lambda: stream in self.headers)
        assert self.headers[stream][":status"] == "200"
        assert self.headers[stream]["sec-websocket-protocol"] == "chat"
        return stream


def exercise(host, mode):
    peer = Peer(host, mode != "off")
    with peer.socket:
        if mode == "off":
            ordinary = peer.request("/check", websocket=False)
            host.command("respond")
            peer.until(lambda: ordinary in peer.ended)
            assert peer.bodies[ordinary] == b"sibling"
            host.command("finish")
            return
        if mode == "multiplex":
            unflagged = peer.request("/ws")
            peer.until(lambda: unflagged in peer.ended)
            assert peer.headers[unflagged][":status"] == "404"
        host.command("publish")
        host.command("check-refusal")
        first = peer.open_websocket(host, 0)
        if mode == "multiplex":
            second = peer.open_websocket(host, 1)
            peer.websocket(first, 1, b"hello")
            host.command("hold 0")
            ordinary = peer.request("/check", websocket=False)
            peer.reset(first)
            # Keep the borrowed payload valid even though its stream lost ownership.
            host.command("reset-held 0")
            peer.websocket(second, 1, b"hello")
            host.command("echo 1")
            host.command("respond")
            peer.until(lambda: ordinary in peer.ended and len(peer.bodies[second]) >= 7)
            assert peer.bodies[ordinary] == b"sibling"
            assert peer.bodies[second] == b"\x81\x05world"
            peer.websocket(second, 2, b"bytes")
            host.command("binary 1")
            peer.websocket(second, 9, b"pulse")
            host.command("ping 1")
            peer.until(lambda: len(peer.bodies[second]) >= 21)
            assert peer.bodies[second][7:] == b"\x82\x05bytes\x8a\x05pulse"
            pending = peer.request("/ws")
            host.command("capture")
            peer.websocket(pending, 1, b"hello")
            peer.reset(pending)
            host.command("cancelled")
            unflagged = peer.request("/check")
            peer.until(lambda: unflagged in peer.ended)
            assert peer.headers[unflagged][":status"] == "404"
            host.command("close 1")
            peer.until(lambda: peer.bodies[second].endswith(b"\x88\x02\x03\xe8"))
            peer.websocket(second, 8, b"\x03\xe8", end=True)
            peer.until(lambda: second in peer.ended)
            assert peer.bodies[second].endswith(b"\x88\x02\x03\xe8")
            host.command("closed 1 1000")
            third = peer.open_websocket(host, 2)
            peer.websocket(third, 8, b"", end=True)
            peer.until(lambda: third in peer.ended)
            assert peer.bodies[third] == b"\x88\x00"
            host.command("closed 2 1005")
            host.command("empty")
        elif mode == "graceful":
            ordinary = peer.request("/check", websocket=False)
            host.command("hold-request")
            host.command("drain")
            peer.until(lambda: bool(peer.goaway))
            late = peer.request("/ws")
            peer.until(lambda: len(peer.goaway) == 2 and late in peer.resets)
            assert peer.goaway[0] == 0x7FFFFFFF and peer.goaway[1] >= ordinary
            assert peer.resets[late] == ErrorCodes.REFUSED_STREAM
            # The listener has also stopped accepting new TLS handshakes.
            try:
                with socket.create_connection(("127.0.0.1", host.port), timeout=0.15) as raw:
                    context = ssl.create_default_context(cafile=str(host.trust_root))
                    context.set_alpn_protocols(["h2"])
                    with context.wrap_socket(raw, server_hostname="localhost"):
                        raise AssertionError("Drain admitted a new TLS connection")
            except (TimeoutError, ConnectionRefusedError, ConnectionResetError, ssl.SSLEOFError):
                pass
            host.command("respond-held")
            peer.until(lambda: ordinary in peer.ended)
            assert peer.bodies[ordinary] == b"sibling"
            peer.websocket(first, 8, b"\x03\xe8", end=True)
            peer.until(lambda: first in peer.ended)
            assert peer.bodies[first] == b"\x88\x02\x03\xe8"
            host.command("closed 0 1000")
            # Participate in the transport's TLS close_notify handshake.
            peer.socket.unwrap().close()
            host.command("wait 2")
        elif mode == "expired":
            host.command("drain")
            peer.until(lambda: bool(peer.goaway))
            assert peer.goaway[0] == 0x7FFFFFFF
            host.command("wait 3")
            host.command("closed 0 1006")
        elif mode == "shutdown":
            peer.websocket(first, 1, b"hello")
            host.command("hold 0")
            host.command("drain")
            host.command("stop")
            host.command("wait 4")
            host.command("release")
            host.command("closed 0 1006")
        host.command("finish")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("server", type=Path)
    parser.add_argument("mode", choices=["off", "multiplex", "graceful", "expired", "shutdown"])
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="h2-ws-", dir=Path.cwd()) as directory:
        host = Host(str(args.server.resolve()), directory, args.mode)
        try:
            exercise(host, args.mode)
            assert host.process.wait(timeout=7) == 0
        finally:
            host.close()
    print(f"PASS installed HTTP/2 WebSocket {args.mode}")


if __name__ == "__main__":
    main()
