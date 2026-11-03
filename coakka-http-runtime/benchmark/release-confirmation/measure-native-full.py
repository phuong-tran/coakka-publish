"""Confirm the Native C fixture against the released SDK and record three admitted runs."""

import gzip
import hashlib
import json
import math
import os
import re
import signal
import subprocess
import time
import urllib.request
from pathlib import Path

root = Path.cwd()
out = root / "full-retest-runs"
out.mkdir()
lib = (root / "sdk/lib/libcoakka_http_host.so.1.1.2").resolve()
fixture = (root / "fixture").resolve()
sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
assert sha(lib) == "25686f3805f5344631d6d66dff351df3cdac577a84d558f8ab7e86f1f42ef7c8"
url = "http://127.0.0.1:18431/fixed"
governor_files = sorted(
    Path("/sys/devices/system/cpu").glob("cpu[0-3]/cpufreq/scaling_governor")
)
original_governors = [path.read_text().strip() for path in governor_files]


def governors(values):
    for path, value in zip(governor_files, values):
        subprocess.run(
            ["sudo", "-n", "tee", path],
            input=value,
            text=True,
            stdout=subprocess.DEVNULL,
            check=True,
        )


def thermal(phase):
    t = int(Path("/sys/class/thermal/thermal_zone0/temp").read_text()) / 1000
    th = subprocess.check_output(["vcgencmd", "get_throttled"], text=True).strip()
    trace.append(dict(phase=phase, c=t, throttle=th))
    return th == "throttled=0x0"


def load(name, seconds, args=()):
    with (run_dir / (name + ".log")).open("w") as f:
        proc = subprocess.Popen(
            [
                "taskset",
                "-c",
                "2-3",
                "h2load",
                "--h1",
                "-c",
                "64",
                "-t",
                "2",
                "-D",
                f"{seconds}s",
                *args,
                url,
            ],
            stdout=f,
            stderr=subprocess.STDOUT,
        )
        try:
            deadline = time.monotonic() + seconds + 10
            while proc.poll() is None:
                assert thermal(name) and time.monotonic() < deadline
                time.sleep(0.5)
            assert proc.returncode == 0 and thermal(name + "-end")
        finally:
            if proc.poll() is None:
                proc.kill()
                proc.wait()


rows = []
try:
    for attempt in range(1, 7):
        if len(rows) == 3:
            break
        run_dir = out / str(attempt)
        run_dir.mkdir()
        trace = []
        try:
            good = 0
            for _ in range(150):
                good = good + 1 if thermal("admission") and trace[-1]["c"] <= 44 else 0
                if good == 3:
                    break
                time.sleep(2)
            else:
                raise RuntimeError("cooldown timeout")
            governors(["performance"] * len(governor_files))
            with (run_dir / "server.log").open("w") as f:
                server = subprocess.Popen(
                    ["taskset", "-c", "0-1", fixture, "18431", "full", "32", "64", "2"],
                    stdout=f,
                    stderr=subprocess.STDOUT,
                    env={
                        k: v
                        for k, v in os.environ.items()
                        if k not in ("LD_PRELOAD", "LD_LIBRARY_PATH")
                    },
                )
                try:
                    for _ in range(50):
                        assert server.poll() is None
                        try:
                            with urllib.request.urlopen(url, timeout=1) as response:
                                assert (
                                    response.read() == b"0123456789abcdef0123456789abcdef"
                                )
                            break
                        except OSError:
                            time.sleep(0.1)
                    else:
                        raise RuntimeError("readiness timeout")
                    # Compare the running executable and mapped SDK before load.
                    maps = Path(f"/proc/{server.pid}/maps").read_text()
                    assert {
                        line.split()[-1]
                        for line in maps.splitlines()
                        if "libcoakka_http_host" in line
                    } == {str(lib)}
                    assert Path(
                        f"/proc/{server.pid}/exe"
                    ).resolve() == fixture and os.sched_getaffinity(server.pid) == {0, 1}
                    assert all(
                        os.sched_getaffinity(int(t.name)) <= {0, 1}
                        for t in Path(f"/proc/{server.pid}/task").iterdir()
                    )
                    (run_dir / "identity.json").write_text(
                        json.dumps({str(lib): sha(lib), str(fixture): sha(fixture)})
                    )
                    log = (run_dir / "server.log").read_text()
                    assert (
                        "host-batch-config request=64 terminal=32" in log
                        and "host-execution configured=2 active=2" in log
                    )
                    load("warmup", 5)
                    assert (
                        thermal("pre-timed") and trace[-1]["c"] < 50
                    ), "timed-load admission requires <50 C"
                    load("timed", 10, ("--log-file=" + str(run_dir / "requests.tsv"),))
                    status = Path(f"/proc/{server.pid}/status").read_text()
                    (run_dir / "process-status.txt").write_text(status)
                finally:
                    if server.poll() is None:
                        server.send_signal(signal.SIGTERM)
                        try:
                            server.wait(timeout=12)
                        except subprocess.TimeoutExpired:
                            server.kill()
                            server.wait()
                assert server.returncode == 0
            # Preserve the raw timing trace and derive percentiles from HTTP 200s.
            latencies_us = []
            with (run_dir / "requests.tsv").open("rb") as f, gzip.open(
                run_dir / "requests.tsv.gz", "wb"
            ) as z:
                for line in f:
                    z.write(line)
                    _, code, us = line.split()
                    assert code == b"200"
                    latencies_us.append(int(us))
            (run_dir / "requests.tsv").unlink()
            log = (run_dir / "timed.log").read_text()
            assert "0 failed, 0 errored, 0 timeout" in log
            latencies_us.sort()
            metric = (
                lambda name: int(re.search(r"^" + name + r":\s+(\d+)", status, re.M)[1])
                / 1024
            )
            rows.append(
                dict(
                    attempt=attempt,
                    requests=len(latencies_us),
                    req_s=len(latencies_us) / 10,
                    p50_ms=latencies_us[math.ceil(len(latencies_us) * 0.50) - 1] / 1000,
                    p95_ms=latencies_us[math.ceil(len(latencies_us) * 0.95) - 1] / 1000,
                    p99_ms=latencies_us[math.ceil(len(latencies_us) * 0.99) - 1] / 1000,
                    rss_mib=metric("VmRSS"),
                    peak_rss_mib=metric("VmHWM"),
                    start_c=next(t["c"] for t in trace if t["phase"] == "pre-timed"),
                    max_c=max(t["c"] for t in trace if t["phase"].startswith("timed")),
                )
            )
            print(rows[-1], flush=True)
        except (AssertionError, RuntimeError) as e:
            (run_dir / "rejected.txt").write_text(repr(e))
            print("rejected", attempt, repr(e), trace[-1], flush=True)
            if trace[-1]["phase"] not in ("pre-timed", "timed", "timed-end") or (
                trace[-1]["phase"].startswith("timed")
                and trace[-1]["throttle"] == "throttled=0x0"
            ):
                raise
        finally:
            governors(original_governors)
            (run_dir / "thermal.json").write_text(json.dumps(trace, indent=2))
finally:
    governors(original_governors)
    restored = [
        server.read_text().strip() for server in governor_files
    ] == original_governors
    (out / "summary.json").write_text(
        json.dumps(dict(rows=rows, governors_restored=restored), indent=2)
    )
assert len(rows) == 3 and restored
