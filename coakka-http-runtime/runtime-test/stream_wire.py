"""Use a real curl to verify the response from the installed public C ABI."""

import queue
import subprocess
import sys
import tempfile
import threading
from concurrent.futures import ThreadPoolExecutor
from concurrent.futures import TimeoutError as FutureTimeoutError
from pathlib import Path


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def read_markers(process, markers):
    for line in process.stdout:
        markers.put(line.rstrip())
    markers.put(None)


def check_response(mode, version, output):
    response, _, negotiated = output.rpartition(b"\n")
    expected_status = {"sse": 200, "stream": 202, "upload": 201}[mode]
    require(negotiated == f"{version} {expected_status}".encode(),
            f"Unexpected negotiation or status: {negotiated!r}")
    head, separator, payload = response.partition(b"\r\n\r\n")
    if mode == "upload":
        require(separator and payload == b"uploaded", "Incorrect upload acknowledgement")
        return
    fields = head.lower().splitlines()
    require(separator and b"x-stream: yes" in fields, f"Invalid head: {head!r}")
    if mode == "sse":
        require(any(line.startswith(b"content-type: text/event-stream") for line in fields),
                f"Missing SSE content type: {fields!r}")
        events = payload.split(b"\n\n")
        require(len(events) == 4 and events[0] == b"data: first" and events[2] == b"data: third" and not events[3],
                f"Invalid SSE event framing: {payload!r}")
        data = [line for line in events[1].splitlines() if line.startswith(b"data: ")]
        metadata = {line for line in events[1].splitlines() if not line.startswith(b"data: ")}
        require(data == [b"data: second", b"data: line"] and
                metadata == {b"event: progress", b"id: 2", b"retry: 1250"},
                f"Invalid SSE event fields: {events[1]!r}")
        return
    body = b"one-two-three"
    require(payload.startswith(body), f"Incomplete or incorrect body: {payload!r}")
    require(payload[len(body):].lower().splitlines() == [b"x-finished: yes"],
            f"Missing or incorrect trailer: {payload[len(body):]!r}")


def request_with_held_data(command, payload, owner, markers):
    with subprocess.Popen(
        command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    ) as client, ThreadPoolExecutor(max_workers=1) as worker:
        result = worker.submit(client.communicate, input=payload, timeout=7)
        try:
            require(markers.get(timeout=6) == "HELD", "Missing first DATA lease")
            # Give the listener a chance to receive further bytes while the sole
            # application DATA slot is held. The marker establishes ownership.
            try:
                _, error = result.result(timeout=0.2)
            except FutureTimeoutError:
                pass
            else:
                raise RuntimeError(f"curl closed before DATA release: {error!r}")
            owner.stdin.write("release\n")
            owner.stdin.flush()
            output, error = result.result(timeout=7)
            return subprocess.CompletedProcess(command, client.returncode, output, error)
        finally:
            if client.poll() is None:
                client.kill()


def cancel_with_held_data(command, owner, markers):
    with subprocess.Popen(
        command, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    ) as client:
        try:
            require(markers.get(timeout=6) == "HELD", "Missing first DATA lease")
            require(client.poll() is None, "Client closed before cancellation")
            client.kill()
            client.wait(timeout=3)
            owner.stdin.write("release\n")
            owner.stdin.flush()
            require(markers.get(timeout=12) == "CANCELLED", "Missing one cancel and one terminal")
        finally:
            if client.poll() is None:
                client.kill()


def exercise(mode, version, curl, certificate, key, consumer,
             upload_size="4096", retained_items="0", reader_mode=None):
    option = {"2": "--http2-prior-knowledge", "3": "--http3-only"}[version]
    fixture = [consumer, mode, version, certificate, key]
    if mode == "upload":
        fixture.extend((upload_size, retained_items))
    if reader_mode is not None:
        require(mode == "upload" and reader_mode in ("hold-first-data", "cancel-held-data"),
                "Invalid reader mode")
        fixture.append(reader_mode)
    with subprocess.Popen(
        fixture,
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True,
    ) as owner:
        markers = queue.Queue()
        reader = threading.Thread(target=read_markers, args=(owner, markers), daemon=True)
        reader.start()
        try:
            ready = markers.get(timeout=8)
            require(ready is not None and ready.startswith("READY "), f"Unexpected listener status: {ready}")
            port = int(ready.removeprefix("READY "))
            require(0 < port <= 65535, "Invalid listener port")
            path = "upload" if mode == "upload" else "stream"
            payload = bytes(i % 251 for i in range(int(upload_size))) if mode == "upload" else None
            request_options = (
                ["--data-binary", "@-", "--header", "Content-Type: application/octet-stream"]
                if mode == "upload" else ["--header", "TE: trailers"]
            )
            command = [
                curl, "-q", "--silent", "--show-error", "--noproxy", "*", "--cacert", certificate,
                option, "--max-time", "5", *request_options, "--dump-header", "-",
                "--write-out", "\n%{http_version} %{response_code}", f"https://127.0.0.1:{port}/{path}",
            ]
            if reader_mode == "hold-first-data":
                result = request_with_held_data(command, payload, owner, markers)
            elif reader_mode == "cancel-held-data":
                # A regular file gives curl the exact length up front. Throttle
                # this test-only sender so the peer is still uploading at HELD.
                command = [
                    curl, "-q", "--silent", "--show-error", "--noproxy", "*",
                    "--cacert", certificate, option, "--max-time", "10",
                    "--request", "POST", "--limit-rate", "4K",
                    "--header", "Expect:", f"https://127.0.0.1:{port}/{path}",
                ]
                with tempfile.TemporaryDirectory(dir=".") as directory:
                    upload = Path(directory) / "upload.bin"
                    upload.write_bytes(payload)
                    cancel_with_held_data([*command, "--upload-file", str(upload)],
                                          owner, markers)
                result = None
            else:
                result = subprocess.run(command, capture_output=True, input=payload, timeout=7)
            if result is not None:
                require(result.returncode == 0,
                        f"curl exit {result.returncode}: {result.stderr!r}; received {result.stdout!r}")
                check_response(mode, version, result.stdout)
                require(markers.get(timeout=8) == "SENT", "Missing matching exchange terminal")
            owner.stdin.write("stop\n")
            owner.stdin.flush()
            require(markers.get(timeout=8) == "STOPPED", "Missing terminal drain and shutdown")
            require(owner.wait(timeout=4) == 0, "The public C consumer failed after shutdown")
        finally:
            if owner.poll() is None:
                owner.kill()
            owner.wait(timeout=4)
            reader.join(timeout=2)


if __name__ == "__main__":
    exercise(*sys.argv[1:])
