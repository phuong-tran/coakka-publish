#!/usr/bin/env python3
"""Check the Pi 5 evidence and the Markdown tables rendered from it."""

import argparse
import json
import math
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parent
CAMPAIGNS = {
    "confirmation": ("release-confirmation", "benchmark-runs-rpi5.md", (
        "native", "go", "node", "bun", "python", "jvm")),
    "framework": ("framework", "framework-runs-rpi5.md", (
        "go-coakka", "go-chi", "go-gin", "node-coakka", "node-express", "node-fastify",
        "bun-coakka", "bun-elysia", "bun-hono", "python-coakka", "python-fastapi", "python-starlette")),
    "bun-pair": ("bun-serve", "bun-serve-runs-rpi5.md", ("bun-coakka", "bun-direct")),
    "cpu": ("node-bun-cpu", "node-bun-cpu-runs-rpi5.md", (
        "node-coakka", "node-fastify", "bun-coakka", "bun-direct")),
    "jvm": ("jvm", "jvm-full-runs-rpi5.md", (
        "coakka", "tomcat", "jetty", "webflux", "mvc-tomcat", "undertow", "vertx")),
}
LABELS = {
    "native": "Native C", "go": "Go", "node": "Node.js", "bun": "Bun",
    "python": "Python", "jvm": "JVM",
    "go-coakka": "CoAkka Go", "go-chi": "Chi 5.3.2", "go-gin": "Gin 1.12.0",
    "node-coakka": "CoAkka Node.js", "node-express": "Express 5.2.1",
    "node-fastify": "Fastify 5.12.4",
    "bun-coakka": "CoAkka Bun", "bun-direct": "Bun.serve()",
    "bun-elysia": "Elysia 1.4.30", "bun-hono": "Hono 4.13.7",
    "python-coakka": "CoAkka Python", "python-fastapi": "FastAPI + Uvicorn",
    "python-starlette": "Starlette + Uvicorn",
    "coakka": "CoAkka JVM", "tomcat": "Tomcat 10.1.50 (direct servlet)",
    "jetty": "Jetty 12.1.13", "webflux": "Spring WebFlux + Reactor Netty",
    "mvc-tomcat": "Spring MVC + Tomcat", "undertow": "Undertow 2.3.26.Final",
    "vertx": "Vert.x Web 4.5.34",
}
CORE = "b2390336057fab867e95d6e3abe50f994a4f85d7c0ee9e91d4fa24a63243b02b"
LOAD_GENERATOR = "5090ade148c09fd5b692cefabbfb1b6e219066c5e14b190205ab15f22c507720"


def require(condition, detail):
    if not condition:
        raise ValueError(detail)


def peak_temperature(row):
    return max((sample["temp_mC"] for sample in row["thermal"]
                if sample["phase"] != "cooldown"), default=None)


def verify_run(row, campaign):
    key = f"{campaign}/{row['attempt']}"
    require(row["round"] >= 1, key)
    expected_warmup = (
        40 if campaign == "jvm" and row["round"] == 4
        else 30 if campaign == "jvm" or (campaign == "confirmation" and row["name"] == "jvm")
        else 5
    )
    require(row["warmup_seconds"] == expected_warmup, f"{key}: warm-up duration")
    if not row["admitted"]:
        require(bool(row.get("reason")), f"{key}: missing rejection reason")
        return

    load_prefix = ["taskset", "-c", "2-3", "/usr/bin/h2load", "--h1", "-c", "64", "-t", "2"]
    timed = row["timed_argv"]
    if campaign == "confirmation":
        timing = ["-D", "10s"]
        require(row["warmup_argv"] == load_prefix + ["-D", f"{expected_warmup}s", timed[-1]],
                f"{key}: warm-up command")
    elif campaign == "cpu":
        timing = ["-D", "17s"]
    else:
        timing = ["-m", "1", "--duration=10s", f"--warm-up-time={expected_warmup}s"]
    require(timed[:-1] == load_prefix + timing + ["--log-file=REQUEST_LOG"]
            and re.fullmatch(r"http://127\.0\.0\.1:1843[012]/fixed", timed[-1]),
            f"{key}: timed load command")

    require(row["identity_stable"], f"{key}: loaded identity drift")
    generator = row["generator_cpu"]
    peak_cpu = max(value for sample in [generator["overall"], *generator["intervals"]]
                   for value in sample.values())
    require(row["generator_peak_percent"] == peak_cpu < 90, f"{key}: generator headroom")
    require("Application protocol: http/1.1" in row["timed_h2load"], f"{key}: measured protocol")
    status = re.search(r"status codes:\s+(\d+) 2xx,\s+0 3xx,\s+0 4xx,\s+0 5xx",
                       row["timed_h2load"])
    require(status is not None and int(status[1]) == row["http_200_total"], f"{key}: h2load status")
    samples = [sample for sample in row["thermal"] if sample["phase"] != "cooldown"]
    cutoff = row.get("thermal_cutoff_mC", 50000)
    require(0 < cutoff <= 50000 and bool(samples) and all(sample["temp_mC"] < cutoff
            and sample["throttled"] == "throttled=0x0" and sample["fan_state"] == 4
            for sample in samples), f"{key}: thermal admission")
    coakka = campaign == "confirmation" or row["name"] == "coakka" or row["name"].endswith("-coakka")
    if coakka:
        core = [item["sha256"] for item in row["loaded"]["native"]
                if item["name"].startswith("libcoakka_http_host")]
        require(core == [CORE], f"{key}: wrong Core image")
        require(row["loaded"]["active_io_loops"] == (2 if row["name"] == "native" else 1),
                f"{key}: unexpected I/O loop count")
    metrics = row["metrics"]
    histogram = metrics["latency_us_counts"]
    requests = metrics["requests"]
    require(requests > 0 and sum(count for _, count in histogram) == requests
            and all(latency >= 0 and count > 0 for latency, count in histogram)
            and sorted(set(latency for latency, _ in histogram)) == [latency for latency, _ in histogram],
            f"{key}: latency frequencies")
    require(metrics["req_s"] == requests / 10, f"{key}: throughput window")
    require(row["http_200_total"] >= requests and (campaign == "cpu" or row["http_200_total"] == requests),
            f"{key}: HTTP 200 accounting")
    for percentile in (50, 95, 99):
        target = (requests * percentile + 99) // 100
        cumulative = 0
        for latency, count in histogram:
            cumulative += count
            if cumulative >= target:
                require(metrics[f"p{percentile}_ms"] == latency / 1000, f"{key}: p{percentile} drift")
                break
    if campaign == "cpu":
        require(row["cpu_seconds"] > 0 and math.isclose(
            metrics["requests_per_cpu_second"], requests / row["cpu_seconds"], rel_tol=1e-12),
            f"{key}: CPU efficiency denominator")


def load_evidence(campaign):
    filename, _, fixtures = CAMPAIGNS[campaign]
    data = json.loads((ROOT / f"{filename}-evidence-rpi5.json").read_text())
    require((data["schema"], data["campaign"], data["connector_version"],
             data["core_sdk_version"], data["c_abi"]) == (2, campaign, "2.2.1", "2.2.0", 10),
            f"{campaign}: release identity")
    require(data["host"]["governors_restored"] and data["host"]["h2load_sha256"] == LOAD_GENERATOR,
            f"{campaign}: host record")
    require(data["workload"] == {
        "protocol": "HTTP/1.1", "path": "/fixed", "connections": 64, "response_bytes": 32,
        "measurement_seconds": 10, "server_cpus": [0, 1], "generator_cpus": [2, 3],
    }, f"{campaign}: workload drift")
    lock = {name: digest for digest, name in
            (line.split() for line in (ROOT / "release-confirmation/inputs.sha256").read_text().splitlines())}
    require(data["packages_sha256"] == lock, f"{campaign}: release input digests")
    expected = {(name, n) for name in fixtures for n in range(1, 5 if campaign == "jvm" else 4)}
    rows = data["runs"]
    require({(row["name"], row["round"]) for row in rows} == expected, f"{campaign}: missing or extra scheduled runs")
    admitted = [(row["name"], row["round"]) for row in rows if row["admitted"]]
    require(len(admitted) == len(set(admitted)), f"{campaign}: duplicate admitted result")
    require(len({row["attempt"] for row in rows}) == len(rows), f"{campaign}: duplicate attempt ID")
    if campaign != "jvm":
        require(set(admitted) == expected, f"{campaign}: missing admitted result")
    for row in rows:
        verify_run(row, campaign)
    return rows


def markdown(headers, rows):
    def line(items):
        return "| " + " | ".join(map(str, items)) + " |"

    return "\n".join([line(headers), line(["---"] * len(headers)), *(line(row) for row in rows)])


def summary(campaign, rows):
    fixtures = CAMPAIGNS[campaign][2]
    rank_by = "requests_per_cpu_second" if campaign == "cpu" else "req_s"
    result = []
    for fixture in fixtures:
        runs = [row for row in rows if row["name"] == fixture and row["admitted"]]
        measured = sorted((row for row in runs if row["round"] < 4),
                          key=lambda row: row["metrics"][rank_by])
        if len(measured) != 3:
            result.append([LABELS[fixture], "—", "—", "—", "Thermal cutoff"])
            continue
        median = measured[1]
        rate = median["metrics"]["req_s"]
        if campaign == "jvm":
            warmup = next((row for row in runs if row["round"] == 4), None)
            if not warmup or abs(warmup["metrics"]["req_s"] / rate - 1) > .05:
                result.append([LABELS[fixture], "—", "—", "—", "Warm-up check did not qualify"])
                continue
        if campaign == "cpu":
            other = f"{median['metrics'][rank_by]:,.1f}"
        else:
            rates = [row["metrics"]["req_s"] for row in measured]
            other = f"{min(rates):,.1f}–{max(rates):,.1f}"
        result.append([LABELS[fixture], f"{rate:,.1f}", other, f"{median['metrics']['p99_ms']:.3f}",
                       f"{max(peak_temperature(row) for row in runs) / 1000:.2f}"])
    second = "Req/CPU-s" if campaign == "cpu" else "Req/s range"
    return markdown(["Fixture", "Median req/s", second, "p99 (ms)", "Peak °C / status"], result)


def attempts(campaign, rows):
    result = []
    for row in sorted(rows, key=lambda row: (row["round"], row["name"], row["attempt"])):
        metrics = row.get("metrics", {})
        peak = peak_temperature(row)
        status = "Admitted" if row["admitted"] else row["reason"].replace("|", "\\|")
        label = LABELS[row["name"]]
        if campaign == "cpu" and row["admitted"]:
            label = f'<a name="{row["name"]}-run-{row["round"]}"></a>{label}'
        result.append([
            label, row["attempt"], row["warmup_seconds"],
            f"{metrics['req_s']:,.1f}" if row["admitted"] else "—",
            f"{metrics['p99_ms']:.3f}" if row["admitted"] else "—",
            f"{row['cpu_seconds']:.5f}" if "cpu_seconds" in row else "—",
            f"{peak / 1000:.2f}" if peak is not None else "—", status,
        ])
    return markdown(["Fixture", "Attempt", "Warm-up (s)", "Req/s", "p99 (ms)",
                     "CPU (s)", "Peak °C", "Status"], result)


def update_block(path, label, content, write):
    text = path.read_text()
    pattern = re.compile(rf"^(<!-- {label}:start -->\n).*?^(<!-- {label}:end -->)$", re.M | re.S)
    rendered, count = pattern.subn(lambda match: match[1] + content + "\n" + match[2], text)
    require(count == 1, f"{path.name}: missing or duplicate {label} block")
    if write:
        path.write_text(rendered)
    else:
        require(rendered == text, f"{path.name}: {label} table drift")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--docs", type=Path, default=ROOT, help="directory containing the benchmark Markdown")
    parser.add_argument("--write", action="store_true", help="render tables into the owning documentation")
    args = parser.parse_args()
    for campaign, (_, page, _) in CAMPAIGNS.items():
        rows = load_evidence(campaign)
        target = args.docs / page
        if campaign != "confirmation":
            update_block(target, "summary", summary(campaign, rows), args.write)
        update_block(target, "attempts", attempts(campaign, rows), args.write)
        if campaign == "confirmation":
            update_block(args.docs / "README.md", "summary", summary(campaign, rows), args.write)
    print("Verified five Pi benchmark datasets and their Markdown tables")


if __name__ == "__main__":
    main()
