#!/usr/bin/env python3
"""Replay the Pi HTTP/1.1 campaigns and retain one receipt per attempt."""

import argparse
import collections
import fcntl
import hashlib
import http.client
import json
import math
import os
from pathlib import Path
import re
import signal
import subprocess
import time

BODY = b"0123456789abcdef0123456789abcdef"
TEMP = Path("/sys/class/thermal/thermal_zone0/temp")
FAN = Path("/sys/class/thermal/cooling_device0/cur_state")
GOVERNORS = sorted(Path("/sys/devices/system/cpu/cpufreq").glob("policy*/scaling_governor"))
CORE_SHA = "b2390336057fab867e95d6e3abe50f994a4f85d7c0ee9e91d4fa24a63243b02b"
H2LOAD_SHA = "5090ade148c09fd5b692cefabbfb1b6e219066c5e14b190205ab15f22c507720"


class ThermalLimit(RuntimeError):
    pass


def sha(path):
    with open(path, "rb") as source:
        return hashlib.file_digest(source, "sha256").hexdigest()


def write_json(path, value):
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n")


def read_cpu():
    rows = Path("/proc/stat").read_text().splitlines()
    return {
        row[0]: (sum(map(int, row[1:9])), int(row[4]) + int(row[5]))
        for line in rows
        if (row := line.split())[0] in ("cpu", "cpu2", "cpu3")
    }


def cpu_busy(before, after, names):
    return {
        name: 100 * (1 - (after[name][1] - before[name][1]) / (after[name][0] - before[name][0]))
        for name in names
        if after[name][0] > before[name][0]
    }


def thermal(phase, samples, strict=True):
    reading = int(TEMP.read_text())
    throttle = subprocess.check_output(["vcgencmd", "get_throttled"], text=True).strip()
    fan = int(FAN.read_text())
    samples.append({"phase": phase, "monotonic": time.monotonic(),
                    "temp_mC": reading, "throttled": throttle, "fan_state": fan})
    if throttle != "throttled=0x0":
        raise RuntimeError(f"Pi throttle state {throttle}")
    if fan != 4:
        raise RuntimeError(f"unexpected fan state {fan}")
    # A Pi temperature step can be 0.55 C. Stop before the next step
    # can cross the campaign's 50 C limit.
    if strict and reading >= 49600:
        raise ThermalLimit(f"{phase} hit thermal cutoff at {reading / 1000:.2f} C")
    return reading


def cooldown(samples, ceiling_mC):
    started = time.monotonic()
    deadline = started + 210
    # The three one-second observations end at or after the cooldown minimum.
    time.sleep(19)
    clean = 0
    while time.monotonic() < deadline:
        start = read_cpu()
        time.sleep(1)
        busy = cpu_busy(start, read_cpu(), ["cpu"])["cpu"]
        reading = thermal("cooldown", samples, strict=False)
        samples[-1]["cpu_busy_percent"] = busy
        clean = clean + 1 if reading <= ceiling_mC and busy <= 5 else 0
        if clean >= 3 and time.monotonic() - started >= 30:
            return
        time.sleep(4)
    raise ThermalLimit("cooldown admission timed out")


def process_status(pid):
    return {
        key: int(match[1])
        for key in ("VmRSS", "VmHWM")
        if (match := re.search(rf"^{key}:\s+(\d+) kB", Path(f"/proc/{pid}/status").read_text(), re.M))
    }


def process_cpu_ticks(pid):
    fields = Path(f"/proc/{pid}/stat").read_text().rsplit(")", 1)[1].split()
    return int(fields[11]) + int(fields[12])


def tree(pid):
    pending, seen = [pid], set()
    while pending:
        child = pending.pop()
        seen.add(child)
        pending.extend(int(n) for n in Path(f"/proc/{child}/task/{child}/children").read_text().split())
    return seen


def probe(port):
    client = http.client.HTTPConnection("127.0.0.1", port, timeout=0.3)
    try:
        client.request("GET", "/fixed")
        with client.getresponse() as response:
            assert response.status == 200 and response.version == 11
            assert response.getheader("Content-Type") == "application/octet-stream"
            assert response.read() == BODY
    finally:
        client.close()


def running_identity(server, lane, log):
    pid = server.pid
    actual_exe = Path(f"/proc/{pid}/exe").resolve(strict=True)
    expected_exe = Path(lane["argv"][0]).resolve(strict=True)
    assert actual_exe == expected_exe, (actual_exe, expected_exe)
    assert sha(actual_exe) == lane["exe_sha256"], actual_exe
    for child in tree(pid):
        for thread in Path(f"/proc/{child}/task").iterdir():
            assert os.sched_getaffinity(int(thread.name)) <= {0, 1}, thread.name
    for path, expected in lane["files"].items():
        assert sha(path) == expected, path
    mapping = Path(f"/proc/{pid}/maps").read_text().splitlines()
    actual_native = {
        line[line.index("/"):] for line in mapping if "/" in line and "coakka_http" in line
    }
    assert actual_native == set(lane.get("native", ())), (actual_native, lane.get("native"))
    for path in actual_native:
        assert sha(path) == lane["native"][path], path
        if "libcoakka_http_host" in path:
            assert lane["native"][path] == lane.get("core_sha256", CORE_SHA), path
    for path in lane.get("framework_native", ()):
        assert any(line.endswith(" " + path) for line in mapping), path
    class_log = lane.get("class_log")
    if class_log:
        # The JVM is asked to report the actual source of its loaded classes.
        loaded_classes = Path(class_log).read_text()
        assert " source: file:" in loaded_classes
    fixture_log = log.read_text()
    if lane.get("coakka") == "native":
        assert "host-batch-config request=64 terminal=32" in fixture_log
        assert "host-execution configured=2 active=2" in fixture_log
    elif lane.get("coakka"):
        records = dict(re.findall(r"^identity-(?:fixture|module|image)=([^\t]+)\t([0-9a-f]{64})$", fixture_log, re.M))
        assert records == {**lane["files"], **lane["native"]}, (records, lane["files"])
        if lane["coakka"] == "go":
            assert "identity-module-version=v2.2.1" in fixture_log
        info = json.loads(re.search(r"^coakka-runtime-info=(.*)$", fixture_log, re.M)[1])
        def values(obj, key):
            if not isinstance(obj, dict):
                return []
            direct = [v for k, v in obj.items() if re.sub("[^a-z]", "", k.lower()) == key]
            return direct + [x for v in obj.values() for x in values(v, key)]
        for key, expected in (("activeeventloops", 1), ("selectedcpucount", 2),
                              ("requestnotificationbatchsize", 64), ("terminalnotificationbatchsize", 32)):
            assert expected in values(info, key), (key, info)
    else:
        info = None
    return {"executable": str(actual_exe), "sha256": lane["exe_sha256"],
            "native": lane.get("native", {}), "files": lane["files"],
            "runtime_info": info if lane.get("coakka") != "native" else None}


def load(base, command, phase, seconds, server, samples, resources, cpu_clock=False):
    start_cpu = last_cpu = read_cpu()
    started = last_sample = time.monotonic()
    cpu_intervals, members = [], None
    if cpu_clock:
        clock = time.clock_gettime_ns
        # Linux MAKE_PROCESS_CPUCLOCK(pid, CPUCLOCK_SCHED).
        process_clock = (~server.pid << 3) | 2
    with (base / f"{phase}.log").open("w") as log:
        client = subprocess.Popen(command, stdout=log, stderr=subprocess.STDOUT,
                                  start_new_session=True)
        try:
            while True:
                current = time.monotonic()
                if cpu_clock:
                    resources["clock"].append((time.time_ns(), clock(process_clock)))
                if current - last_sample >= 1:
                    now_cpu = read_cpu()
                    cpu_intervals.append(cpu_busy(last_cpu, now_cpu, ["cpu2", "cpu3"]))
                    last_sample, last_cpu = current, now_cpu
                thermal(phase, samples)
                if resources["sample_rss"]:
                    current_members = tree(server.pid)
                    if members is None:
                        members = current_members
                    assert members == current_members, "server worker membership changed"
                    rss = sum(process_status(pid)["VmRSS"] for pid in members)
                    resources["rss"].append((current - started, rss))
                    resources["process_ticks"].append(sum(process_cpu_ticks(pid) for pid in members))
                result = client.poll()
                if result is not None:
                    assert result == 0, f"h2load exited {result}"
                    break
                if current - started > seconds + 12:
                    raise TimeoutError(f"{phase} exceeded deadline")
                if cpu_clock:
                    # Keep the clock bracket tight near the 5s and 15s boundaries.
                    elapsed = time.monotonic() - started
                    if abs(elapsed - 5) < 0.4 or abs(elapsed - 15) < 0.4:
                        for _ in range(100):
                            time.sleep(0.002)
                            resources["clock"].append((time.time_ns(), clock(process_clock)))
                        continue
                time.sleep(0.24)
        finally:
            if client.poll() is None:
                os.killpg(client.pid, signal.SIGTERM)
                client.wait(timeout=5)
    thermal(f"{phase}-end", samples)
    overall = cpu_busy(start_cpu, read_cpu(), ["cpu2", "cpu3"])
    return {"overall": overall, "intervals": cpu_intervals}


def timed_counts(log):
    text = log.read_text()
    request = re.search(r"requests:\s+\d+ total,\s+\d+ started,\s+(\d+) done,\s+(\d+) succeeded,\s+0 failed,\s+0 errored,\s+0 timeout", text)
    status = re.search(r"status codes:\s+(\d+) 2xx,\s+0 3xx,\s+0 4xx,\s+0 5xx", text)
    assert request and status, text[-1000:]
    assert int(request[1]) == int(request[2]) == int(status[1]) > 0
    return int(status[1])


def reduce_log(path, expected, window=None):
    histogram, count = collections.Counter(), 0
    with path.open() as source:
        for row in source:
            started, status, usec = map(int, row.split())
            assert status == 200, row
            count += 1
            if window is None or window[0] <= started + usec < window[1]:
                histogram[usec] += 1
    assert count == expected, (count, expected)
    total = histogram.total()
    assert total > 0
    ordered = sorted(histogram.items())
    percentiles = {}
    for label, fraction in (("p50", 0.50), ("p95", 0.95), ("p99", 0.99)):
        seen = 0
        for latency, frequency in ordered:
            seen += frequency
            if seen >= math.ceil(total * fraction):
                percentiles[f"{label}_ms"] = latency / 1000
                break
    return {"requests": total, "req_s": total / 10, "latency_us_counts": ordered, **percentiles}


def interpolate(samples, instant):
    for (left, cpu_left), (right, cpu_right) in zip(samples, samples[1:]):
        if left <= instant <= right:
            assert right - left < 35_000_000, "CPU deadline bracket exceeded 35 ms"
            return cpu_left + (cpu_right - cpu_left) * (instant - left) / (right - left)
    raise RuntimeError("missing CPU deadline bracket")


def run_attempt(lane, base, original_governors):
    receipt = {"name": lane["name"], "campaign": lane["campaign"],
               "round": lane["round"], "admitted": False,
               "fixture_note": lane.get("fixture_note"), "runner_sha256": sha(__file__)}
    samples = []
    server = None
    try:
        ceiling_mC = lane.get("cooldown_ceiling_mC", 43000)
        receipt["cooldown_ceiling_mC"] = ceiling_mC
        receipt["thermal_cutoff_mC"] = 49600
        cooldown(samples, ceiling_mC)
        governors(["performance"] * len(original_governors))
        env = os.environ.copy()
        for key in ("LD_PRELOAD", "LD_LIBRARY_PATH", "NODE_OPTIONS", "BUN_OPTIONS",
                    "JAVA_TOOL_OPTIONS", "JDK_JAVA_OPTIONS", "COAKKA_CPU_PROFILE"):
            env.pop(key, None)
        env.update(lane.get("env", {}))
        argv = ["taskset", "-c", "0-1", *lane["argv"]]
        receipt.update(server_argv=argv, server_env=lane.get("env", {}))
        with (base / "server.log").open("w") as server_log:
            server = subprocess.Popen(argv, cwd=lane.get("cwd"), env=env, stdout=server_log,
                                      stderr=subprocess.STDOUT, start_new_session=True)
        deadline = time.monotonic() + 20
        while True:
            thermal("startup", samples)
            assert server.poll() is None, "server exited before readiness"
            try:
                probe(lane["port"])
                if not lane.get("coakka") or lane["coakka"] == "native" or "coakka-runtime-info=" in (base / "server.log").read_text():
                    break
            except (OSError, http.client.HTTPException):
                pass
            if time.monotonic() >= deadline:
                raise TimeoutError("server readiness deadline")
            time.sleep(0.25)
        receipt["before"] = running_identity(server, lane, base / "server.log")
        url = f"http://127.0.0.1:{lane['port']}/fixed"
        request_log = base.resolve() / "requests.tsv"
        common = ["taskset", "-c", "2-3", "/usr/bin/h2load", "--h1", "-c", "64", "-t", "2"]
        resources = {"sample_rss": lane["campaign"] not in ("confirmation", "cpu"), "rss": [], "clock": [], "process_ticks": []}
        warmup = lane["warmup"]
        cpu = lane["campaign"] == "cpu"
        if lane["campaign"] == "confirmation":
            warmup_cmd = [*common, "-D", f"{warmup}s", url]
            receipt["warmup_argv"] = warmup_cmd
            load(base, warmup_cmd, "warmup", warmup, server, samples, resources)
            timed = [*common, "-D", "10s", f"--log-file={request_log}", url]
            seconds = 10
        elif cpu:
            timed = [*common, "-D", "17s", f"--log-file={request_log}", url]
            seconds = 17
        else:
            timed = [*common, "-m", "1", "--duration=10s", f"--warm-up-time={warmup}s",
                     f"--log-file={request_log}", url]
            seconds = 10 + warmup
        receipt["timed_argv"] = timed
        receipt["generator"] = load(base, timed, "timed", seconds, server, samples, resources, cpu)
        generator = receipt["generator"]
        assert generator["intervals"] and len(generator["overall"]) == 2
        peak_cpu = max(v for sample in [generator["overall"], *generator["intervals"]] for v in sample.values())
        receipt["generator_peak_percent"] = peak_cpu
        receipt["rss_status_kib"] = process_status(server.pid)
        receipt["after"] = running_identity(server, lane, base / "server.log")
        expected = timed_counts(base / "timed.log")
        receipt["http_200_total"] = expected
        window = None
        if cpu:
            with request_log.open() as source:
                first_start_us = min(int(line.split(maxsplit=1)[0]) for line in source)
            window = first_start_us + 5_000_000, first_start_us + 15_000_000
            left, right = [interpolate(resources["clock"], bound * 1000) for bound in window]
            receipt["cpu_window"] = {"window_us": window, "cpu_seconds": (right - left) / 1e9,
                                     "samples": resources["clock"]}
        receipt["metrics"] = reduce_log(request_log, expected, window)
        receipt["request_log_sha256"] = sha(request_log)
        request_log.unlink()
        if cpu:
            receipt["metrics"]["requests_per_cpu_second"] = receipt["metrics"]["requests"] / receipt["cpu_window"]["cpu_seconds"]
        if resources["rss"]:
            rss = resources["rss"]
            elapsed = rss[-1][0] - rss[0][0]
            mean = sum((b[0] - a[0]) * (a[1] + b[1]) / 2 for a, b in zip(rss, rss[1:])) / elapsed
            receipt["rss_sampled_kib"] = {"mean": mean, "peak": max(value for _, value in rss), "samples": rss}
            cpu_seconds = (resources["process_ticks"][-1] - resources["process_ticks"][0]) / os.sysconf("SC_CLK_TCK")
            receipt["server_cpu_percent"] = 100 * cpu_seconds / elapsed
        if peak_cpu >= 90:
            receipt["reason"] = "generator CPU reached 90%"
    except (Exception, KeyboardInterrupt) as err:
        receipt["reason"] = repr(err)
    finally:
        if server is not None:
            if server.poll() is None:
                os.killpg(server.pid, signal.SIGTERM)
                try:
                    server.wait(timeout=15)
                except subprocess.TimeoutExpired:
                    os.killpg(server.pid, signal.SIGKILL)
                    server.wait()
                    receipt["reason"] = "server shutdown timed out"
            receipt["server_exit"] = server.returncode
            log = (base / "server.log").read_text()
            if lane.get("coakka") == "native":
                counts = re.search(r"host-event-accounting requests=(\d+) terminals=(\d+) other=(\d+)", log)
                if counts:
                    receipt["terminal_counts"] = list(map(int, counts.groups()))
                if server.returncode != 0 or not counts or int(counts[2]) < int(counts[1]) or int(counts[1]) < receipt.get("http_200_total", 0):
                    receipt.setdefault("reason", "native terminal accounting or shutdown failed")
            elif lane.get("coakka") and ("benchmark-shutdown=pass" not in log or server.returncode not in (0, -15, 143)):
                receipt.setdefault("reason", "CoAkka connector shutdown failed")
            elif not lane.get("coakka") and (server.returncode not in (0, -15, 143) or Path(f"/proc/{server.pid}").exists()):
                receipt.setdefault("reason", "framework shutdown failed")
        governors(original_governors)
        receipt["thermal"] = samples
        receipt["admitted"] = "reason" not in receipt
        write_json(base / "receipt.json", receipt)
    return receipt


def governors(values):
    for path, value in zip(GOVERNORS, values, strict=True):
        subprocess.run(["sudo", "-n", "tee", str(path)], input=value, text=True,
                       stdout=subprocess.DEVNULL, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("schedule", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    os.sched_setaffinity(0, {2, 3})
    schedule = json.loads(args.schedule.read_text())
    args.output.mkdir(exist_ok=True)
    with Path(".replay.lock").open("w") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        original = [path.read_text().strip() for path in GOVERNORS]
        assert original and sha("/usr/bin/h2load") == H2LOAD_SHA
        host = {"uname": os.uname().release, "machine": os.uname().machine,
                "board": Path("/proc/device-tree/model").read_text().rstrip("\0"),
                "governors_before": original, "governors_restored": False,
                "h2load_sha256": H2LOAD_SHA, "runner_sha256": sha(__file__),
                "schedule_sha256": sha(args.schedule), "fan_state": int(FAN.read_text()),
                "os_release": Path("/etc/os-release").read_text(),
                "firmware": subprocess.check_output(["vcgencmd", "version"], text=True)}
        write_json(args.output / "host.json", host)
        interrupted = False
        def stop(signum, _frame):
            nonlocal interrupted
            interrupted = True
            raise KeyboardInterrupt(f"signal {signum}")
        signal.signal(signal.SIGTERM, stop)
        signal.signal(signal.SIGINT, stop)
        try:
            for lane in schedule:
                prefix = f"{lane['round']:02d}-{lane['name']}"
                if any(json.loads(path.read_text())["admitted"] for path in args.output.glob(f"{prefix}-*/receipt.json")):
                    continue
                for number in range(1, lane.get("attempt_limit", 4) + 1):
                    base = args.output / f"{prefix}-{number}"
                    if base.exists():
                        continue
                    base.mkdir()
                    receipt = run_attempt(lane, base, original)
                    print(prefix, "admitted" if receipt["admitted"] else receipt["reason"],
                          receipt.get("metrics", {}).get("req_s"), flush=True)
                    if interrupted:
                        return
                    if receipt["admitted"]:
                        break
                    reason = receipt.get("reason", "")
                    if "ThermalLimit" not in reason and "generator CPU" not in reason:
                        if lane["campaign"] == "confirmation":
                            raise SystemExit(f"{prefix} failed; see receipt")
                        break
                    if "generator CPU" in reason and lane["campaign"] != "confirmation":
                        break
        finally:
            governors(original)
            host["governors_restored"] = original == [path.read_text().strip() for path in GOVERNORS]
            write_json(args.output / "host.json", host)
            assert host["governors_restored"]


if __name__ == "__main__":
    main()
