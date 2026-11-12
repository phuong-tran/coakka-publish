"""Run version-forced loopback requests against the installed SDK fixture."""

import argparse
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import subprocess
import tempfile

FLAGS = {"1.1": "--http1.1", "2": "--http2", "3": "--http3-only"}


def probe(curl, directory, url, version):
    result = subprocess.run(
        [curl, "-q", "--silent", "--show-error", "--noproxy", "*",
         "--cacert", "root.pem", FLAGS[version], "--max-time", "8",
         "--write-out", "\n%{http_code} %{http_version}", url],
        cwd=directory, capture_output=True, text=True, timeout=10,
    )
    if result.returncode != 0:
        raise RuntimeError(f"HTTP/{version} curl exited {result.returncode}: {result.stderr}")
    expected = f"protocol-ok\n200 {version}"
    if result.stdout != expected:
        raise RuntimeError(f"Expected {expected!r}, received {result.stdout!r}")
    return f"HTTP/{version}"


def check_endpoint(fixture, curl, profile):
    versions = list(FLAGS) if profile == "mixed" else [profile]
    with tempfile.TemporaryDirectory(prefix="http-protocols-", dir=Path.cwd()) as work:
        with subprocess.Popen(
            [fixture, profile], cwd=work, stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, text=True,
        ) as server, ThreadPoolExecutor(max_workers=3) as pool:
            try:
                line = pool.submit(server.stdout.readline, 160).result(timeout=10)
                ready = json.loads(line)
                port = ready["port"]
                if type(port) is not int or not 0 < port <= 65535:
                    raise RuntimeError(f"Fixture did not publish a valid port: {port!r}")
                url = f"https://127.0.0.1:{port}/protocol"
                requests = [pool.submit(probe, curl, work, url, version)
                            for version in versions]
                observed = [request.result() for request in requests]
                stdout, _ = server.communicate(input="\n", timeout=10)
                receipt = json.loads(stdout)
                if (server.returncode != 0 or not receipt["ok"] or
                        not receipt["cleanup"] or receipt["abi"] != 10):
                    raise RuntimeError(f"Listener teardown failed: {stdout!r}")
                report = {"endpoint": url, "profile": profile,
                          "observed_protocols": observed, "abi": receipt["abi"]}
                if profile == "mixed":
                    report["admitting_protocols"] = ready["admitting_protocols"]
                return report
            finally:
                if server.poll() is None:
                    server.kill()
                    server.wait(timeout=5)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("fixture", type=Path)
    parser.add_argument("curl", type=Path)
    parser.add_argument("profile", choices=[*FLAGS, "mixed"])
    args = parser.parse_args()
    print(json.dumps(check_endpoint(args.fixture.resolve(strict=True),
                                    args.curl.resolve(strict=True), args.profile)))


if __name__ == "__main__":
    main()
