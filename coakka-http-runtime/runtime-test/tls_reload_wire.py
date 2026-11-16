"""Verify live credential rotation through the installed host ABI and curl."""

import argparse
import subprocess
from pathlib import Path
from tempfile import TemporaryDirectory


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
