# Raspberry Pi 5 Benchmark Methodology

## Contents

- [Status](#status)
- [Comparison Scope](#comparison-scope)
- [Machine Record](#machine-record)
- [Workload And CPU Budgets](#workload-and-cpu-budgets)
- [Warm-Up And Cooldown](#warm-up-and-cooldown)
- [Metrics And Accounting](#metrics-and-accounting)
- [Acceptance And Reproduction](#acceptance-and-reproduction)

## Status

Selected installed r3 cohorts have verified localhost observations in the
[results ledger](benchmark-results-rpi5.md). Unmeasured final-package profiles
and framework comparisons remain Pending. Historical source-build peaks are
not substituted for current package evidence.

## Comparison Scope

| Host | Measured application | Comparison applications |
| --- | --- | --- |
| Native C | CoAkka public callback API | None; standalone native baseline |
| Go | CoAkka host-inlined | Chi, Gin |
| Kotlin/JVM | CoAkka host-inlined | Spring WebFlux, Spring MVC with Tomcat, Jetty, Undertow, Vert.x |
| Python | CoAkka host-inlined | FastAPI, Starlette; exact server stack named |
| Node.js | CoAkka host-inlined | Express, Fastify |
| Bun | CoAkka host-inlined | Elysia, Hono |

Vert.x and Undertow comparisons are **Pending** for both CPU budgets. The
current JVM remeasurement covers CoAkka only; no accepted comparison results
are published for those two frameworks in this round.

No standalone Netty or native-library competitor is included. The separately
requested Bun.serve paired study is reported explicitly; it does not add other
language-standard HTTP servers to the comparison matrix.
Framework dependencies are not extra comparison rows.
Spring MVC with Tomcat is one stack, not two independent results. C++ samples
refer to the native C baseline without claiming a separate C++ measurement.

## Machine Record

Capture these facts from the Pi at campaign start and attach them to every
result through a campaign identifier. Do not reuse a former machine snapshot
as proof of the current boot environment.

| Category | Required facts |
| --- | --- |
| Board | Raspberry Pi model/revision; physical machine, not emulation |
| CPU | Architecture, logical CPU count, CPU model, affinity sets, governor, frequency limits and observed frequency |
| Memory | Installed/OS-visible RAM, available RAM before each run, swap configuration/activity |
| Operating system | Distribution/release, kernel, firmware identity, boot command line where relevant |
| Storage | Boot device/model and filesystem; evidence/output location |
| Cooling | Fan policy, ambient temperature if measured, board temperature before/after each run, throttle flags |
| Software | Exact CoAkka archive hashes, language/framework/load-tool versions and startup flags |
| Effective configuration | Core-reported CPU/loop/batch/timeout/backend state, alongside requested configuration |
| Network | Loopback or external topology, interfaces, HTTP/TLS mode, client placement |

Unknown values are explicitly unavailable, never guessed. Exclude credentials,
private keys, unrelated environment variables and other sensitive machine data.

## Workload And CPU Budgets

The initial fixed-response workload is `GET /fixed`, HTTP/1.1 keep-alive,
status `200`, `Content-Type: application/octet-stream`, and the 32-byte body
`0123456789abcdef0123456789abcdef`. Verify bytes and headers before timing.
No database, logging, compression, TLS, proxy or application authentication is
silently added to only one implementation. This workload is a transport and
handler-overhead measurement, not production application capacity.

Measure independent 1-CPU and 2-CPU profiles. Record actual CPU IDs, all server
workers/threads, runtime flags and the generator CPU set. Server and generator
CPU sets must not overlap in a loopback run. An allowed two-CPU set does not
prove that a single-threaded host uses both CPUs; show observed CPU usage.
Do not silently add clustered workers to only one side. If a supported worker
configuration is used, label it as a distinct profile. Missing profiles say
**not measured**, not zero throughput.

Freeze concurrency, warm-up, measurement duration, repetitions, tool arguments,
CPU sets, framework settings and acceptance thresholds before the campaign.
Use at least three accepted repeated rounds with reproducible balanced order.
Validate generator headroom: saturation makes the server result inconclusive.

## Warm-Up And Cooldown

Warm each process before its timed interval. Qualify JVM/JIT warm-up stability;
a short fixed delay alone is not evidence of steady state. Record the chosen
warm-up and observed stability instead of applying unequal hidden warm-up.

Before **every** run, including the first and retries, stop the preceding
server and wait at least 15 seconds, until board temperature is at most 50 °C,
background CPU busy is at most 5%, and throttle status is clear. Freeze the
sampling interval and number of consecutive qualifying observations in the
campaign configuration. The current gate requires three clean observations,
with a five-second wait between one-second CPU samples; any failure resets the
streak. A bounded cooldown deadline expiring rejects or defers
the run; it never bypasses the gate. Keep cooling policy fixed throughout.

Warm-up can heat the CPU again: record conditions at the start and end of the
timed interval too. Any throttling during measurement rejects that run.

## Metrics And Accounting

| Metric | Definition and reporting rule |
| --- | --- |
| Throughput | Successful, workload-valid responses per measured second; retain per-run values, median and range |
| Latency | Client-observed p50/p95/p99 with units and measurement method; do not invent percentiles from an average |
| Outcomes | Total completed, valid success, wrong status/body, connection errors, timeouts and incomplete requests |
| CPU | User/system CPU time for the complete server process tree; 100% means one logical CPU, so two busy CPUs approach 200% |
| RSS | Sampled mean and peak in MiB for all server processes, with sampling interval and measurement window |
| Memory caveat | Summed process RSS can double-count shared pages; it is not unique physical-memory usage or an instantaneous unsampled peak |
| Scheduling | Voluntary/involuntary context switches where available; separate intrusive profiling from throughput measurements |
| Environment | Before/after temperature, throttling, observed frequencies and background activity |
| Generator | Separate CPU/RSS and errors; never include generator resources in the server totals |

Include supervisors and workers consistently; record how exited workers are
accounted for. Missing metrics say **N/A — reason**, not zero. Record collector
cost. Never merge latency histograms incorrectly or average percentiles while
labelling the result as a pooled percentile: report per-run percentiles or use
compatible raw histograms.

## Acceptance And Reproduction

Every result needs: candidate identities, fixture/config identity, full commands,
raw output and telemetry, timestamps, accepted/rejected status and reason.
Retain rejected attempts rather than selecting only the fastest run. A workload
mismatch, unexplained errors, throttling, cooldown failure, missing required
telemetry or saturated generator prevents publication of that run.

Publish the machine table, per-profile result tables and raw evidence together.
Compare only matched workloads and CPU budgets. Do not use old records as new
results, rank unrelated ecosystems by this tiny workload, or substitute a
throughput win for feature/lifecycle correctness.
