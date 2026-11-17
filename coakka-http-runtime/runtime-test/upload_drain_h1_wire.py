"""Replay the standalone HTTP/1.1 upload expiry contract through ABI 10."""

from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import socket
import ssl
import subprocess
import sys


def connect(port, security, certificate):
    peer = socket.create_connection(("127.0.0.1", port), timeout=5)
    if security == "plain":
        return peer
    # The isolated loopback test pins the exact fixture leaf certificate.
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    context.check_hostname = False
    context.verify_mode = ssl.CERT_NONE
    context.set_alpn_protocols(["http/1.1"])
    with_socket = context.wrap_socket(peer, server_hostname="localhost")
    pem = certificate.read_text().split("-----END CERTIFICATE-----", 1)[0]
    expected = ssl.PEM_cert_to_DER_cert(pem + "-----END CERTIFICATE-----")
    if (with_socket.getpeercert(binary_form=True) != expected or
            with_socket.selected_alpn_protocol() not in (None, "http/1.1")):
        with_socket.close()
        raise RuntimeError("unexpected loopback HTTP/1.1 TLS identity")
    return with_socket


def run_case(binary, certificate, key, scenario, security, close_peer):
    # Ask Core to start on a fixed port on both starts. Port-zero profiles
    # can use different OS socket-reuse rules.
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as reservation:
        reservation.bind(("127.0.0.1", 0))
        port = reservation.getsockname()[1]
    command = [binary, scenario, security, certificate, key, str(port)]
    with subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          text=True) as owner, ThreadPoolExecutor(max_workers=1) as executor:
        def marker():
            return executor.submit(owner.stdout.readline).result(timeout=8).strip()

        try:
            ready = marker()
            if ready != f"PORT={port}":
                raise RuntimeError(f"missing bound port: {ready!r}")
            with connect(port, security, certificate) as ready_peer:
                ready_peer.sendall(b"GET /ready HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
                payload = bytearray()
                while not payload.endswith(b"ready"):
                    chunk = ready_peer.recv(4096)
                    if not chunk or len(payload) + len(chunk) > 4096:
                        raise RuntimeError(f"incomplete healthy response: {bytes(payload)!r}")
                    payload.extend(chunk)
                if not payload.startswith(b"HTTP/1.1 200 "):
                    raise RuntimeError(f"unexpected healthy response: {bytes(payload)!r}")
            if marker() != "READY":
                raise RuntimeError("healthy request terminal was not observed")
            with connect(port, security, certificate) as upload:
                head = b"POST /upload HTTP/1.1\r\nHost: localhost\r\n"
                if scenario == "trailers":
                    upload.sendall(head + b"Transfer-Encoding: chunked\r\nTrailer: X-Receipt\r\n\r\n"
                                   b"4\r\naaaa\r\n0\r\nX-Receipt: ready\r\n\r\n")
                else:
                    upload.sendall(head + b"Content-Length: 65536\r\n\r\n" + b"a" * 4096)
                if marker() != "EXPIRED":
                    raise RuntimeError("upload did not drive EXPIRED")
                if close_peer:
                    with upload.makefile("rb") as response:
                        line = response.readline(256)
                        if not line.startswith(b"HTTP/1.1 503 "):
                            raise RuntimeError(f"unexpected expiry response: {line!r}")
                    upload.close()
                owner.stdin.write("release\n")
                owner.stdin.flush()
                if marker() != "STOPPED":
                    raise RuntimeError("Core did not complete stop and destroy")
                upload.close()
                owner.stdin.write("reopen\n")
                owner.stdin.flush()
                details, _ = owner.communicate(timeout=9)
                print(details, end="", flush=True)
                if owner.returncode or f"PASS {scenario} {security} reopened={port}" not in details:
                    raise RuntimeError(f"failed stop/reopen: {scenario}, {security}, peer_closed={close_peer}")
                print(f"HTTP/1.1 {security} {scenario} EXPIRED/stop/reopen peer_closed={close_peer}",
                      flush=True)
        finally:
            if owner.poll() is None:
                owner.kill()


def main():
    binary, certificate, key = (Path(value).resolve() for value in sys.argv[1:])
    for security in ("plain", "tls"):
        for scenario in ("held", "returned", "trailers"):
            for close_peer in (False, True):
                run_case(binary, certificate, key, scenario, security, close_peer)


if __name__ == "__main__":
    main()
