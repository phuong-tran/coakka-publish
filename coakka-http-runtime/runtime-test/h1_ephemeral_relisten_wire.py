"""Exercise immediate standalone HTTP/1.1 reopen through an installed SDK."""

import argparse
from concurrent.futures import ThreadPoolExecutor
import errno
import json
from pathlib import Path
import socket
import ssl
import subprocess
from tempfile import TemporaryDirectory


def first_port(address, explicit):
    if not explicit:
        return 0
    family = socket.AF_INET6 if address == "::1" else socket.AF_INET
    with socket.socket(family, socket.SOCK_STREAM) as probe:
        probe.bind((address, 0))
        return probe.getsockname()[1]


def reply(peer):
    peer.sendall(b"GET /relisten HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
    response = bytearray()
    while data := peer.recv(4096):
        response.extend(data)
        if len(response) > 16384:
            raise RuntimeError("Test response exceeded its byte bound")
    head, boundary, body = response.partition(b"\r\n\r\n")
    if not head.startswith(b"HTTP/1.1 200 ") or not boundary or body != b"relisten-ok":
        raise RuntimeError(f"Unexpected response: {response!r}")


def request(address, port, security, directory):
    with socket.create_connection((address, port), timeout=6) as transport:
        if security == "plain":
            reply(transport)
            return
        context = ssl.create_default_context(cafile=str(directory / "root.pem"))
        context.set_alpn_protocols(["http/1.1"])
        if security == "mtls":
            context.load_cert_chain(directory / "certificate.pem", directory / "key.pem")
        with context.wrap_socket(transport, server_hostname="localhost") as peer:
            negotiated = peer.selected_alpn_protocol()
            if negotiated not in (None, "http/1.1"):
                raise RuntimeError(f"Negotiated the wrong ALPN: {negotiated!r}")
            reply(peer)


def check_closed(address, port):
    family = socket.AF_INET6 if address == "::1" else socket.AF_INET
    with socket.socket(family, socket.SOCK_STREAM) as probe:
        probe.settimeout(3)
        status = probe.connect_ex((address, port))
    if status not in (errno.ECONNREFUSED, 10061):
        raise RuntimeError(f"Stopped listener did not refuse the connection: errno {status}")


def exercise(fixture, address, security, initial, traffic):
    start = first_port(address, initial == "fixed")
    with TemporaryDirectory(prefix="h1-relisten-", dir=Path.cwd()) as scratch:
        directory = Path(scratch)
        with subprocess.Popen(
            [fixture, address, security, str(start), traffic], cwd=directory,
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1,
        ) as server, ThreadPoolExecutor(max_workers=1) as reader:
            def receive():
                return reader.submit(server.stdout.readline, 256).result(timeout=15).strip()

            def advance():
                server.stdin.write("\n")
                server.stdin.flush()

            try:
                marker, _, number = receive().partition(" ")
                if marker != "READY" or not number.isdecimal() or not 0 < int(number) <= 65535:
                    raise RuntimeError(f"Unexpected ready line: {marker} {number}")
                port = int(number)
                if traffic == "request":
                    request(address, port, security, directory)
                advance()
                if receive() != "CLOSED":
                    raise RuntimeError("First listener did not close")
                check_closed(address, port)
                # The second start uses exactly the port selected by the first
                # start, with no delay or retry to age out TCP TIME_WAIT.
                advance()
                result = receive()
                if result != f"REOPENED {port}":
                    raise RuntimeError(f"Cannot reopen {address}:{port}: {result!r}")
                request(address, port, security, directory)
                advance()
                receipt = json.loads(receive())
                if server.wait(timeout=6) != 0 or receipt != {
                    "case": "standalone_h1_ephemeral_relisten", "ok": True, "cleanup": True, "abi": 10,
                }:
                    raise RuntimeError(f"Fixture cleanup failed: {receipt!r}")
            finally:
                if server.poll() is None:
                    server.kill()
                    server.wait(timeout=5)
    print(f"PASS HTTP/1.1 {address} {security} {initial} {traffic}: port {port}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("fixture", type=Path)
    parser.add_argument("address", choices=("127.0.0.1", "::1"))
    parser.add_argument("security", choices=("plain", "tls", "mtls"))
    parser.add_argument("initial", choices=("ephemeral", "fixed"))
    parser.add_argument("traffic", choices=("request", "idle"))
    args = parser.parse_args()
    exercise(args.fixture.resolve(strict=True), args.address, args.security, args.initial, args.traffic)


if __name__ == "__main__":
    main()
