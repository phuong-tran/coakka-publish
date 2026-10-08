# Raspberry Pi 5 Benchmark Results

## Contents

- [Results](#results)
  - [Native C baseline](#native-c-baseline)
- [Machine and measurement](#machine-and-measurement)
- [Per-run throughput](#per-run-throughput)
- [Evidence identities](#evidence-identities)

## Results

| Application | CPUs | Req/s median (range) | p50 / p95 / p99 ms | CPU % | RSS mean / sampled peak MiB | Errors / timeouts |
| --- | ---: | --- | --- | ---: | --- | --- |
| Go + CoAkka | 1 | 52,040.3 (51,821.0–53,269.8) | 1.165 / 1.771 / 1.981 | 93.7 | 25.5 / 25.8 | 0 / 0 |
| Python + CoAkka | 1 | 10,532.0 (10,516.0–10,584.3) | 5.865 / 9.669 / 10.526 | 99.8 | 35.2 / 35.2 | 0 / 0 |
| Node.js + CoAkka | 1 | 29,377.8 (29,351.7–29,526.0) | 2.138 / 2.353 / 3.687 | 83.9 | 71.1 / 72.6 | 0 / 0 |
| Go + CoAkka | 2 | 93,169.9 (92,407.1–93,324.7) | 0.662 / 0.830 / 1.213 | 159.0 | 25.9 / 26.4 | 0 / 0 |
| Python + CoAkka | 2 | 11,919.1 (11,845.1–11,985.8) | 5.384 / 6.128 / 6.985 | 137.1 | 35.2 / 35.2 | 0 / 0 |
| Node.js + CoAkka | 2 | 56,487.6 (55,532.0–58,252.8) | 1.099 / 1.310 / 2.134 | 159.5 | 69.3 / 70.0 | 0 / 0 |
| Bun + CoAkka | 1 | 30,561.8 (29,891.5–31,867.3) | 2.065 / 2.425 / 2.744 | 99.5 | 66.8 / 68.1 | 0 / 0 |
| Bun.serve | 1 | 46,201.4 (46,043.5–46,323.7) | 1.334 / 1.765 / 2.158 | 99.8 | 38.7 / 41.7 | 0 / 0 |
| Bun + CoAkka | 2 | 66,026.3 (65,502.2–66,225.1) | 0.944 / 1.063 / 1.585 | 171.0 | 66.1 / 67.1 | 0 / 0 |
| Bun.serve | 2 | 46,521.7 (46,263.5–46,557.1) | 1.330 / 1.741 / 2.186 | 100.3 | 39.7 / 42.0 | 0 / 0 |
| Kotlin/JVM + CoAkka | 2 | 94,918.3 (94,639.6–95,380.8) | 0.664 / 0.700 / 0.922 | 154.8 | 219.4 / 223.9 | 0 / 0 |

### Native C baseline

No competitor is included. This is the installed public C callback surface;
C++ samples reference these C measurements, not an independently measured C++ server.

| Application | CPUs | Req/s median (range) | p50 / p95 / p99 ms | CPU % | RSS mean / sampled peak MiB | Errors / timeouts |
| --- | ---: | --- | --- | ---: | --- | --- |
| Native C + CoAkka | 1 | 61,248.1 (60,692.4–61,400.1) | 1.051 / 1.302 / 1.411 | 99.6 | 9.2 / 9.2 | 0 / 0 |
| Native C + CoAkka | 2 | 93,072.7 (91,968.4–93,349.7) | 0.679 / 0.726 / 0.988 | 151.7 | 9.2 / 9.2 | 0 / 0 |

Both CPU profiles use the runtime-reported one event loop and notification
batches 8/8. CPU budget is not an event-loop count. These results do not
reuse historical throughput peaks from different workloads.

These are three-run **localhost observations**, not unconstrained capacity or
a universal framework ranking. All rows use installed packages admitted as
`2026-10-08-r3`; no package was rebuilt for this documentation update. Go,
Node.js and Python reuse their unchanged qualified cohort. Bun uses the later
paired Bun.serve cohort. JVM uses only the final selected package's two-CPU
cohort; the older JVM archive's results are not relabeled.

Native C is measured separately, without a competitor. C++ refers to
that C baseline, not a separate C++ measurement. Final JVM 1-CPU measurements
remain **Pending**. Chi/Gin,
FastAPI/Starlette, Express/Fastify, Elysia/Hono, Spring WebFlux, Spring MVC with
Tomcat, Jetty, Vert.x and Undertow comparisons remain **Pending** for this
exact-package table; no missing comparison is treated as a win. Vert.x and
Undertow are intentionally not rerun. There is no public Netty comparison.

## Machine and measurement

| Field | Recorded value |
| --- | --- |
| Board / CPU | Raspberry Pi 5 Model B Rev 1.1; four Cortex-A76, 2.4 GHz |
| RAM | 16 GB board; 16,607,536 KiB OS-visible |
| OS / kernel | Debian GNU/Linux 13 Trixie; 6.18.50+rpt-rpi-2712 aarch64; USB root |
| Firmware | 2026-09-25, version 1accd665 |
| Cooling | Maximum fan, state 4; ambient temperature not measured |
| Governor / throttling | Performance during load; original governors restored; throttled=0x0 |
| Workload | IPv4 loopback, plaintext HTTP/1.1, GET /fixed, status 200, 32-byte response |
| Connections | 64 persistent clients, one request in flight per connection |
| Process model | One application process; no Bun cluster/worker variant |
| CPU budget | 1 CPU: server 0, generator 1–3; 2 CPUs: server 0–1, generator 2–3 |
| Duration | Three fresh-process runs; 10s measured; 5s warm-up, JVM 30s |
| Cooldown | At least 30s, <=50°C and busiest CPU <=5%, three consecutive clean samples before each run |
| Runtime settings | Ordinary host-inlined package defaults; no application loop override; notification batches 8/8 |
| Tools | Go 1.27.1; Node 22.23.3; Bun 1.4.2; Python 3.13.5; OpenJDK 21.0.12.1; h2load 1.64.0 |
| Native consumer | C11, GCC 14.2.0, Release with -O3 -DNDEBUG; installed r3 library unchanged |

Tool versions above are benchmark versions, not minimum connector requirements.
CPU100% means one logical CPU. CPU/RSS include warm-up plus measurement. RSS
uses 250ms samples: time-weighted process-tree sum and sampled peak (shared
pages may be counted twice). Table resource values and latency percentiles are
medians of per-run values, not pooled-request percentiles. Context switches
were not measured. No generator saturation was observed at the 90% gate; this
does not prove an unconstrained ceiling. Exact mapped-package checks, cooldown,
post-load readiness and graceful shutdown passed in the retained campaigns.

Bun.serve uses method/path dispatch, URL parsing, shared payload bytes and a
fresh Fetch Response. CoAkka uses its public Builder and reusable immutable
Response. Both use the same Bun binary. CoAkka/Bun.serve median ratios are
0.661 at 1CPU and 1.419 at 2CPU; these describe this paired workload only.

## Per-run throughput

| Application | CPUs | Round 1 / 2 / 3 req/s |
| --- | ---: | --- |
| Go + CoAkka | 1 | 52,040.3 / 51,821.0 / 53,269.8 |
| Python + CoAkka | 1 | 10,516.0 / 10,532.0 / 10,584.3 |
| Node.js + CoAkka | 1 | 29,377.8 / 29,526.0 / 29,351.7 |
| Go + CoAkka | 2 | 92,407.1 / 93,324.7 / 93,169.9 |
| Python + CoAkka | 2 | 11,845.1 / 11,919.1 / 11,985.8 |
| Node.js + CoAkka | 2 | 58,252.8 / 56,487.6 / 55,532.0 |
| Bun + CoAkka | 1 | 30,561.8 / 29,891.5 / 31,867.3 |
| Bun.serve | 1 | 46,043.5 / 46,323.7 / 46,201.4 |
| Bun + CoAkka | 2 | 66,225.1 / 65,502.2 / 66,026.3 |
| Bun.serve | 2 | 46,557.1 / 46,521.7 / 46,263.5 |
| Kotlin/JVM + CoAkka | 2 | 94,639.6 / 94,918.3 / 95,380.8 |
| Native C + CoAkka | 1 | 61,400.1 / 61,248.1 / 60,692.4 |
| Native C + CoAkka | 2 | 91,968.4 / 93,072.7 / 93,349.7 |

## Evidence identities

Raw campaigns, telemetry, failures and tool/package locks are retained in the
private release evidence store. The following hashes identify those campaign
records without exposing local paths or runtime implementation details.
Package hashes are independently listed in each r3 warehouse checksum ledger.

| Campaign / CPU budget | Campaign JSON SHA-256 |
| --- | --- |
| Connector cohort / 1 CPUs | `31640e5a80fbf01260cd6506469f9225c9492b775445457493bfc6b4b3d7f811` |
| Connector cohort / 2 CPUs | `24d8ad37f2b8beee946915ca3a1c10bea7a5c8eb08cede04bc18a12250aa90f4` |
| Bun paired study / 1 CPUs | `4c24cae2dc93fdd4c580b236e6b5454e7dbf262f456e9dd043ffff27ee022636` |
| Bun paired study / 2 CPUs | `355f41ae0d0ebf885bfa065a8b44136be982f8557ab14a2961d77cae42c30d20` |
| JVM selected package / 2 CPUs | `90b38a76fa723bf6f6af7c50133f7f16a481c84ebbd7404dbfa00cd3a303e433` |
| Native C package / 1 CPUs | `05f393733416a5cd62aec42b7abfc4049a9524ef905494dc8b3fcb8ce5f9cfa5` |
| Native C package / 2 CPUs | `469307472211cdb9076c4561a8671c6ed787491ba05297b30026a2096c004852` |
