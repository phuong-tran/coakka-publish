"""Verify Core-owned static and authority-scoped file delivery over real HTTP."""

from pathlib import Path
import queue
import subprocess
import sys
import tempfile
import threading

from stream_wire import read_markers, require


def fetch(curl, version, base, path, certificate, status, body, *, head=False, headers=()):
    option = {"1.1": "--http1.1", "2": "--http2-prior-knowledge", "3": "--http3-only"}[version]
    context = f"{'HEAD' if head else 'GET'} {path}, expected HTTP/{version} {status}"
    command = [curl, "-q", "--silent", "--show-error", "--noproxy", "*",
               option, "--cacert", certificate, "--max-time", "5",
               "--write-out", "\n%{http_version} %{response_code}"]
    if head:
        command.append("--head")
    else:
        command.extend(("--dump-header", "-"))
    for header in headers:
        command.extend(("--header", header))
    result = subprocess.run([*command, base + path], capture_output=True, timeout=7)
    require(result.returncode == 0,
            f"{context}: curl exit {result.returncode}: {result.stderr!r}; "
            f"received {len(result.stdout)} bytes")
    response, _, metadata = result.stdout.rpartition(b"\n")
    require(metadata == f"{version} {status}".encode(), f"{context}: unexpected result {metadata!r}")
    fields, separator, actual = response.partition(b"\r\n\r\n")
    require(separator and (body is None or actual == body),
            f"{context}: wrong body or framing, received {len(actual)} bytes")
    print(f"{context}: received {len(actual)} body bytes", flush=True)
    pairs = (line.split(b": ", 1) for line in fields.splitlines()[1:] if b": " in line)
    return {name.lower(): value for name, value in pairs}


def exercise(version, curl, certificate, key, scratch, consumer):
    with tempfile.TemporaryDirectory(dir=scratch, prefix="http-file-wire-") as folder:
        root = Path(folder)
        asset = bytes(range(256)) * 192
        (root / "asset.txt").write_bytes(asset)
        (root / "index.html").write_bytes(b"<p>SPA fixture</p>")
        with subprocess.Popen([consumer, version, certificate, key, folder],
                              stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True) as owner:
            markers = queue.Queue()
            reader = threading.Thread(target=read_markers, args=(owner, markers), daemon=True)
            reader.start()
            try:
                ready = markers.get(timeout=8)
                require(ready is not None and ready.startswith("READY "), f"Unexpected status: {ready}")
                port = int(ready.removeprefix("READY "))
                require(0 < port <= 65535, "Invalid listener port")
                scheme = "http" if version == "1.1" else "https"
                base = f"{scheme}://127.0.0.1:{port}"

                def get(path, status, body, **kwargs):
                    return fetch(curl, version, base, path, certificate, status, body, **kwargs)

                def send(command):
                    owner.stdin.write(command + "\n")
                    owner.stdin.flush()

                static = "/assets/asset.txt"
                fields = get(static, 200, asset)
                etag = fields.get(b"etag")
                require(etag, "Static file did not advertise an ETag")
                fields = get(static, 304, b"", headers=(f"If-None-Match: {etag.decode('ascii')}",))
                require(fields.get(b"etag") == etag, "304 lost the static ETag")
                fields = get(static, 200, b"", head=True)
                require(fields.get(b"content-length") == b"49152", "HEAD lost the file length")
                fields = get(static, 206, asset[2:6], headers=("Range: bytes=2-5",))
                require(fields.get(b"content-range") == b"bytes 2-5/49152", "Invalid static Content-Range")
                fields = get(static, 416, None, headers=("Range: bytes=99999-",))
                require(fields.get(b"content-range") == b"bytes */49152", "Invalid unsatisfied range")
                get("/assets/client/route", 200, b"<p>SPA fixture</p>", headers=("Accept: text/html",))
                get("/assets/missing.txt", 404, None)
                send("check")
                require(markers.get(timeout=8) == "STATIC", "Static requests reached the handler")

                download_etag = None
                for status, body, method, headers in (
                    (200, asset, False, ()),
                    (206, asset[2:6], False, ("Range: bytes=2-5",)),
                    (200, b"", True, ()),
                ):
                    send("file")
                    fields = get("/download", status, body, head=method, headers=headers)
                    require(fields.get(b"cache-control") == b"private, max-age=60",
                            "Missing handler cache policy")
                    if method:
                        require(fields.get(b"content-length") == b"49152", "File HEAD lost the file length")
                    if status == 206:
                        require(fields.get(b"content-range") == b"bytes 2-5/49152", "Invalid file Content-Range")
                    if status == 200 and not method:
                        download_etag = fields.get(b"etag")
                    require(markers.get(timeout=8) == "SENT", "Missing file response terminal")

                require(download_etag, "Handler-selected file did not advertise an ETag")
                send("file")
                fields = get("/download", 304, b"",
                             headers=(f"If-None-Match: {download_etag.decode('ascii')}",))
                require(fields.get(b"etag") == download_etag, "304 lost the file ETag")
                require(markers.get(timeout=8) == "SENT", "Missing conditional file terminal")

                send("stop")
                owner.stdin.close()
                drain = markers.get(timeout=7)
                fields = drain.split() if drain else []
                require(len(fields) == 4 and fields[0] == "DRAIN",
                        f"Unexpected drain result: {drain}")
                allowed = {"COMPLETE", "EXPIRED"} if version == "3" else {"COMPLETE"}
                require(fields[1] in allowed, f"Unexpected drain outcome: {drain}")
                print(drain, flush=True)
                if version == "3":
                    rebound = markers.get(timeout=7)
                    require(rebound == f"REBOUND {port}", f"Wrong UDP reopen: {rebound}")
                    print(rebound, flush=True)
                require(owner.wait(timeout=7) == 0,
                        "Core file service failed to drain or close")
            finally:
                if owner.poll() is None:
                    owner.kill()
                owner.wait(timeout=4)
                reader.join(timeout=2)
        print(f"HTTP/{version} standalone static files, SPA and authority-scoped file delivery PASS")


if __name__ == "__main__":
    exercise(*sys.argv[1:])
