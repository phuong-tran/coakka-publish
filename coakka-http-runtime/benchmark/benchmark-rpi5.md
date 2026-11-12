# Raspberry Pi 5 Benchmark Methodology

## Contents

- [Status](#status)
- [Comparison scope](#comparison-scope)
- [Machine record](#machine-record)
- [Workload and CPU budgets](#workload-and-cpu-budgets)
- [Warm-up and cooldown](#warm-up-and-cooldown)
- [Metrics and accounting](#metrics-and-accounting)
- [Acceptance and reproduction](#acceptance-and-reproduction)

## Status

This campaign measured the released HTTP Runtime 2.2.1 connectors and
native SDK 2.2.0 with C ABI 10 on November 12, 2026. [Current results](README.md)

## Comparison scope

| Campaign | Workload | Measurements |
| --- | --- | --- |
| Release confirmation | Native C and the five connectors | Three runs per fixture |
| Framework comparison | Go, Node.js, Bun and Python fixtures | Three runs per fixture |
| Bun.serve() | Two Bun 1.4.2 fixtures | Three runs per fixture |
| CPU efficiency | Node.js and Bun fixtures | Three runs per fixture |
| JVM | Seven JVM fixtures | Three measurements and one warm-up check per fixture |

These measurements cover a fixed loopback response. TLS, HTTP/2, HTTP/3,
remote traffic and application logic require separate measurements.

## Machine record

Raspberry Pi 5 Model B Rev 1.1, AArch64, Debian 13.7, kernel
`6.18.50+rpt-rpi-2712` and h2load 1.64.0. The fan remained at state 4.
The CPU governor was set to `performance` during load and restored to
`ondemand` at the end of each campaign.

The release confirmation used Go 1.26.3, Node.js 22.0.0, Bun 1.4.2,
Python 3.13.5 and OpenJDK 21.0.12.1. The framework and CPU efficiency
comparisons used Node.js 22.23.3. The JSON evidence records the host
and file digests.

## Workload and CPU budgets

h2load ran two threads on CPUs 2–3. The server ran on CPUs 0–1.
The generator used 64 persistent HTTP/1.1 connections to `GET /fixed`.
The response was HTTP 200 with a fixed 32-byte binary body. Native C
used two I/O loops and one host event reader. Each CoAkka connector
used one active I/O loop. Framework fixture topology is described
alongside its results.

## Warm-up and cooldown

Each measurement window was 10 seconds. Release confirmation used
separate warm-up and timed connections, with five seconds of warm-up
for C, Go, Node.js, Bun and Python and 30 seconds for JVM. The framework
and Bun.serve() comparisons warmed the measured connections for five
seconds. The JVM comparison used 30 seconds for its three measurements
and 40 seconds for the separate warm-up check. The CPU efficiency fixture ran for
17 seconds and selected the middle 10 seconds after a five-second warm-up.

The runner waited at least 30 seconds for cooldown. It checked CPU idle
time, temperature and firmware throttle state before starting, then
sampled temperature during warm-up and load. Admitted runs stayed below
50°C with no throttling. After an excluded Jetty attempt reached 50.15°C,
the JVM campaign used a 49.60°C cutoff and cooler starts for Jetty,
Undertow and Vert.x.

## Metrics and accounting

- Throughput is the number of successful responses in the 10-second window
  divided by ten. p50, p95 and p99 come from exact latency frequencies.
- Throughput tables use the median-throughput run for p99. The CPU
  efficiency table uses the median requests per CPU-second and reports
  throughput and p99 from that same run.
- Release confirmation records process RSS and high-water mark after
  measurement. Framework comparisons record sampled mean and peak RSS
  during warm-up and load. Both are available in the evidence.
- JVM ranking requires three admitted measurements and an admitted
  40-second warm-up check within 5% of the throughput median.

## Acceptance and reproduction

The harness verifies the response, the process's loaded file identities,
CPU affinity, the I/O loop count and clean shutdown. It admits a run
with complete HTTP 200 accounting, no throttling and peak generator CPU
below 90%. The JVM temperature exclusions remain visible in the run log.

[Evidence files and reproduction commands](evidence-rpi5.md)
