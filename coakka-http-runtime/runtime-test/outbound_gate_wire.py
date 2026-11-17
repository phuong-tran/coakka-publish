"""Use local HTTP/1.1 peers to check outbound reuse, TLS and timeout."""

import argparse
from contextlib import ExitStack, suppress
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import ssl
import subprocess
import threading


class Upstream(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def do_GET(self):
        if self.path == "/short":
            response = b"HTTP/1.1 200 OK\r\nContent-Length: 20\r\nConnection: close\r\n\r\nshort"
        elif self.path == "/large-head":
            response = (b"HTTP/1.1 200 OK\r\nX-Fixture: " + b"x" * (9 * 1024)
                        + b"\r\nContent-Length: 2\r\nConnection: close\r\n\r\nok")
        elif self.path == "/large-body":
            # Send over the pinned SDK's default transport body cap without
            # advertising Content-Length, so this exercises streamed counting.
            response = (b"HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
                        + (b"1000\r\n" + b"x" * 4096 + b"\r\n") * 65 + b"0\r\n\r\n")
        elif self.path in ("/large-declared-body", "/body-boundary"):
            size = 256 * 1024 + (self.path == "/large-declared-body")
            response = (b"HTTP/1.1 200 OK\r\nConnection: close\r\n"
                        + f"Content-Length: {size}\r\n\r\n".encode()
                        + b"x" * size)
        else:
            self.send_error(404)
            return
        self.close_connection = True
        with suppress(OSError):
            self.wfile.write(response)

    def do_POST(self):
        if self.headers.get("x-fixture") != "native" or self.headers.get("Content-Length") != "16":
            self.send_error(400)
            return
        body = self.rfile.read(16)
        if body != b"outbound-payload":
            self.send_error(400)
            return
        if self.path == "/stall":
            self.server.stop_test.wait(timeout=5)
            self.close_connection = True
            return
        if self.path != "/echo":
            self.send_error(404)
            return
        self.send_response(201)
        self.send_header("x-peer-port", str(self.client_address[1]))
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, format, *args):
        pass


def start(stack, tls=None):
    server = stack.enter_context(ThreadingHTTPServer(("127.0.0.1", 0), Upstream))
    server.stop_test = threading.Event()
    if tls is not None:
        server.socket = tls.wrap_socket(server.socket, server_side=True)
    worker = threading.Thread(target=server.serve_forever)
    worker.start()

    def stop():
        server.stop_test.set()
        server.shutdown()
        worker.join(5)
        assert not worker.is_alive()

    stack.callback(stop)
    return server.server_port


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("consumer", type=Path)
    parser.add_argument("certificate", type=Path)
    parser.add_argument("key", type=Path)
    parser.add_argument("--response-case", choices=("short", "large-head", "large-body",
                                                   "large-declared-body", "body-boundary"))
    args = parser.parse_args()
    tls = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    tls.load_cert_chain(args.certificate, args.key)
    with ExitStack() as stack:
        plain = start(stack)
        secure = start(stack, tls)
        command = [args.consumer, str(plain), str(secure), args.certificate]
        if args.response_case:
            command.append(args.response_case)
        subprocess.run(command, check=True, timeout=30)


if __name__ == "__main__":
    main()
