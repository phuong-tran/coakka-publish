"""Verify live credential rotation through the installed host ABI and curl."""

import argparse
from concurrent.futures import ThreadPoolExecutor
from queue import Queue
import socket
import ssl
import subprocess
from threading import Event
from pathlib import Path
from tempfile import TemporaryDirectory


def pending_handshake(port, directory, identity, transition, pending, resume):
    context = ssl.create_default_context(cafile=str(Path(directory) / "a-root.pem"))
    context.load_verify_locations(cafile=str(Path(directory) / "b-root.pem"))
    context.load_cert_chain(str(Path(directory) / f"{identity}-cert.pem"),
                            str(Path(directory) / f"{identity}-key.pem"))
    context.set_alpn_protocols(["h2"])
    with socket.create_connection(("127.0.0.1", port), timeout=4) as peer:
        with context.wrap_socket(peer, server_hostname="127.0.0.1",
                                 do_handshake_on_connect=False) as tls:
            local_port = tls.getsockname()[1]
            tls.setblocking(False)
            try:
                tls.do_handshake()
            except ssl.SSLWantReadError:
                pending.put(local_port)
            else:
                raise AssertionError("expected handshake to wait for the server")

            if not resume.wait(timeout=5):
                raise TimeoutError(f"rotation did not release handshake on port {local_port}")
            tls.settimeout(4)
            try:
                tls.do_handshake()
                if tls.selected_alpn_protocol() != "h2":
                    raise AssertionError("auxiliary peer did not negotiate HTTP/2")
                # A TLS 1.3 peer may receive its authentication alert after
                # do_handshake returns. Try a bounded read on this same socket.
                tls.settimeout(0.2)
                try:
                    if tls.recv(1):
                        return local_port, "HTTP/2 data"
                except socket.timeout:
                    return local_port, "no alert before read deadline"
                return local_port, "closed"
            except ssl.SSLError as error:
                # Some SSL providers report a retired socket as a syscall
                # error instead of a Python connection error.
                if transition == "drain" and error.errno == ssl.SSL_ERROR_SYSCALL:
                    return local_port, "TLS transport closed during drain"
                if error.reason not in {"TLSV1_ALERT_UNKNOWN_CA", "UNEXPECTED_EOF_WHILE_READING"}:
                    raise
                return local_port, error.reason
            except ConnectionError as error:
                # A handshake already in progress can be retired by drain.
                return local_port, type(error).__name__


def rotate_during_handshakes(command, transition, port, directory):
    pending = Queue()
    resume = Event()
    # Each auxiliary TLS socket stays on its own native thread. The held
    # HTTP/2 exchange is owned by a separate curl process.
    with ThreadPoolExecutor(max_workers=4) as workers:
        handshakes = [
            workers.submit(pending_handshake, port, directory, identity,
                           transition, pending, resume)
            for identity in ("a", "b", "a", "b")
        ]
        try:
            ports = {pending.get(timeout=5) for _ in handshakes}
            if len(ports) != len(handshakes):
                raise AssertionError("auxiliary handshakes must use distinct sockets")
            command(transition)
        finally:
            resume.set()
        for handshake in handshakes:
            local_port, outcome = handshake.result(timeout=6)
            print(f"{transition} auxiliary TLS port={local_port} {outcome}")


def exercise(fixture, curl, protocol, security, directory):
    with subprocess.Popen(
        [fixture, protocol, security], cwd=directory,
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True,
    ) as server:
        try:
            port = int(server.stdout.readline())

            def command(value):
                server.stdin.write(value + "\n")
                server.stdin.flush()
                if server.stdout.readline() != "ok\n":
                    raise AssertionError(f"server did not acknowledge {value}")

            def request(trust, client=None, transition=None, reject=False):
                args = [curl, "-q", "--silent", "--show-error", "--noproxy", "*",
                        "--max-time", "4", "--cacert", f"{trust}-root.pem",
                        "--write-out", "\\n%{http_version} %{http_code}",
                        {"1.1": "--http1.1", "2": "--http2",
                         "3": "--http3-only"}[protocol]]
                if client is not None:
                    args.extend(["--cert", f"{client}-cert.pem",
                                 "--key", f"{client}-key.pem"])
                args.append(f"https://127.0.0.1:{port}/tls")
                with subprocess.Popen(
                    args, cwd=directory, stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE, text=True,
                ) as peer:
                    try:
                        if not reject:
                            command("hold")
                            if transition is not None:
                                if protocol == "2" and security == "mtls":
                                    rotate_during_handshakes(command, transition, port, directory)
                                else:
                                    command(transition)
                            command("respond")
                        output, error = peer.communicate(timeout=6)
                        expected = f"rotation-ok\n{protocol} 200"
                        if reject:
                            command("no-request")
                            if (peer.returncode not in {35, 55, 56, 60, 95, 96}
                                    or not output.endswith(" 000")):
                                raise AssertionError(
                                    f"no explicit credential refusal for trust={trust}, "
                                    f"client={client}: exit={peer.returncode}, "
                                    f"{output!r} {error!r}"
                                )
                        elif peer.returncode != 0 or output != expected:
                            raise AssertionError(
                                f"expected {expected!r}, got {output!r}: {error}"
                            )
                        elif transition is not None and protocol == "2" and security == "mtls":
                            command("no-request")
                            print(f"{transition}: held HTTP/2 reply verified and no auxiliary request admitted")
                    finally:
                        if peer.poll() is None:
                            peer.kill()

            mutual = security == "mtls"
            a = "a" if mutual else None
            b = "b" if mutual else None
            command("stale")
            command("bad-key")
            if mutual:
                command("bad-trust")
            request("a", a, transition="keep")
            request("a", b, reject=True)
            if mutual:
                request("b", a, reject=True)
                request("b", reject=True)
            request("b", b, transition="drain")
            request("b", a, reject=True)
            request("a", a)
            command("finish")
            if server.wait(timeout=5) != 0:
                raise AssertionError("server teardown failed")
        finally:
            if server.poll() is None:
                server.kill()
    print(f"HTTP/{protocol} {security}: rollback, keep and drain passed")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("fixture", type=Path)
    parser.add_argument("curl", type=Path)
    parser.add_argument("protocol", choices=("1.1", "2", "3"))
    parser.add_argument("security", choices=("tls", "mtls"))
    args = parser.parse_args()
    with TemporaryDirectory(prefix="coakka-tls-reload-") as directory:
        exercise(str(args.fixture.resolve()), str(args.curl.resolve()),
                 args.protocol, args.security, directory)


if __name__ == "__main__":
    main()
