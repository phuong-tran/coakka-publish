# Raspberry Pi 5 Benchmark Protocol

This protocol governs private CoAkka HTTP Runtime measurements on a physical
Raspberry Pi 5. The comparison matrix was frozen on **2026-09-13**; every result
must also carry its actual measurement date.

No CoAkka language result is accepted unless the measured process proves that
it started the same public service path used by normal applications.

## Contents

- [Questions](#questions)
- [Application Path Rule](#application-path-rule)
- [I/O Backend A/B Rule](#io-backend-ab-rule)
- [Source And Artifact Evidence](#source-and-artifact-evidence)
- [Authority Host](#authority-host)
- [Comparison Matrix](#comparison-matrix)
- [Workloads](#workloads)
- [Run Controls](#run-controls)
- [Metrics](#metrics)
- [Pressure And Lifecycle](#pressure-and-lifecycle)
- [Result Rules](#result-rules)
- [Publication Tables](#publication-tables)
- [Open Gate](#open-gate)

## Questions

The benchmark answers separate, narrow questions:

1. What do the native C and C++ entry points sustain as standalone references?
2. How does each language's CoAkka service compare with direct,
   non-framework HTTP in that same App Host?
3. How does it compare separately with each selected framework/server
   combination in the same ecosystem?
4. For the same CoAkka handler in each language, what changes when an eligible
   Linux HTTP/2 TLS listener selects explicit `io_uring` instead of the
   platform-default I/O backend?
5. What p99 latency, CPU, memory, scheduling, pressure, and shutdown behavior
   accompany throughput?

There is no ranking across programming languages.

## Application Path Rule

For Java, Kotlin, Python, JavaScript, TypeScript, and Go:

- benchmark the public builder/service that normal applications import;
- execute handlers inside the named App Host using its ordinary values and
  scheduling model;
- record `application_path=normal` in the lane config and ready record;
- make the runner reject a missing or mismatched application-path identity;
- do not substitute a historical fast binary or benchmark-only adapter for the
  current product source.

Native C and C++ are direct reference lanes, so they do not need a language
connector label and are not compared with an external framework.

## I/O Backend A/B Rule

The I/O backend experiment is separate from the HTTP/1.1 framework comparison.
It runs one independent pair for Go, JVM, Python, Node.js, and Bun using the
complete `CoAkka HTTP Runtime` surface in that language package.

Both sides of one pair must use:

- the same application source and fixed-response handler;
- HTTP/2 over TLS with the same certificate and trust configuration;
- the same limits, CPU placement, connection count, and parallel streams;
- the same frozen package image and lifecycle;
- one effective difference: platform-default or explicit `io_uring`.

The process must report the effective protocol, security mode, and backend in
its ready record. It must also inspect `/proc/self/fd`: the `io_uring` lane is
accepted only when it acquires a real `io_uring` descriptor, and the baseline is
accepted only when it does not. A requested backend name is not proof that the
kernel path became active.

The current candidate supports explicit `io_uring` only for eligible HTTP/2 and
HTTP/3 server configurations. It is intentionally not injected into the
HTTP/1.1 direct/framework comparison.

## Source And Artifact Evidence

Every reported number ships with:

- the complete application/server source for CoAkka and its comparator;
- exact dependency versions and lock files;
- the precise application package source or immutable package digest;
- compiler/runtime/load-generator versions;
- build commands and dependency information for measured binaries;
- raw load output and per-round lifecycle/resource sidecars;
- host-control state before, during, and after the campaign;
- a deterministic source manifest and final evidence checksum.

Readable source does not replace proof of which bytes executed. A package hash
does not replace readable benchmark source. Both are required.

## Authority Host

The authority host is a physical Raspberry Pi 5. The final evidence must
recapture every value rather than copying an earlier description.

| Field | Required evidence |
| --- | --- |
| Board | Model/revision and stable redacted host label |
| CPU/RAM | Architecture, cores, memory |
| OS | Distribution, kernel, libc |
| Firmware | Firmware and EEPROM versions |
| Storage | Model, transport, filesystem and mounts |
| Cooling/power | Configuration plus start/end temperature |
| CPU policy | Governor and sampled frequency |
| Throttling | Firmware state before, during, and after |
| Background work | Running services, user services, timers, and network state |
| Tools | Exact language runtime, compiler, package, and load-generator versions |

The current lab host is a Raspberry Pi 5 Model B Rev 1.1 with 16 GiB RAM. That
description is context only until a campaign recaptures it.

## Comparison Matrix

| Ecosystem | Independent comparisons |
| --- | --- |
| Native C | Standalone CoAkka native reference |
| Native C++ | Standalone CoAkka native reference |
| JVM | JDK direct HTTP, Netty, bare Tomcat, and bare Jetty, each vs the CoAkka Java service |
| Go | `net/http`, Chi, and Gin, each vs the CoAkka Go service |
| Python | Standard-library direct HTTP, raw Uvicorn, and FastAPI/Uvicorn, each vs the CoAkka Python service |
| JavaScript on Node.js | `node:http`, Express, and Fastify, each vs the CoAkka JavaScript service on Node.js |
| JavaScript on Bun | `Bun.serve`, Elysia, and Hono, each vs the CoAkka JavaScript service on Bun |

Profiles may share a rotating campaign, but reports join comparator and CoAkka
samples from the same workload, concurrency, round, host state, and CPU policy.
They do not turn all profiles into one framework leaderboard.

The backend matrix is independent:

| Ecosystem | Backend comparison | Protocol |
| --- | --- | --- |
| Go | Platform default vs explicit `io_uring` | HTTP/2 TLS |
| JVM | Platform default vs explicit `io_uring` | HTTP/2 TLS |
| Python | Platform default vs explicit `io_uring` | HTTP/2 TLS |
| JavaScript on Node.js | Platform default vs explicit `io_uring` | HTTP/2 TLS |
| JavaScript on Bun | Platform default vs explicit `io_uring` | HTTP/2 TLS |
| Native | Platform default vs explicit `io_uring` | HTTP/2 TLS standalone reference |

## Workloads

| ID | Request | Response | Purpose |
| --- | --- | --- | --- |
| `fixed-32` | `GET /fixed`, no body | Prebuilt 32-byte body, status 200 | Routing and fixed-response floor |
| `echo-128` | `POST /echo`, fixed 128 bytes | Same 128 bytes | Request/reply boundary |
| `echo-1024` | `POST /echo`, fixed 1 KiB | Same 1 KiB | Copy and retained-byte sensitivity |
| `json-route` | Small JSON `POST /items/{id}?mode=fast` | Fixed JSON acknowledgement | Captures, query, headers, body |
| `stream-1m` | `GET /stream` | 1 MiB in 16 KiB chunks | Streaming and writability |
| `fixed-32-http2-tls` | HTTP/2 TLS `GET /fixed`, no body | Prebuilt 32-byte body, status 200 | Same-language platform-default/`io_uring` A/B |

All lanes use keep-alive, equivalent response headers where possible, no access
log, no debug mode, no compression, and the same correctness checks. A lane
that cannot implement the contract is marked not comparable.

Buffered workloads use concurrency `1`, `8`, `32`, and `128`; streaming uses
`1`, `4`, and `16`. A separate pressure campaign intentionally exceeds the
configured capacity.

## Run Controls

1. Build and test every source before reboot.
2. Run a short qualification; verify exact response, application-path identity,
   and plausible throughput before allowing a long campaign.
3. Reboot and start within the recorded clean-boot window.
4. Confirm the host is cool, unthrottled, and using the selected governor.
5. Record then stop only the declared nonessential system and user services.
6. Pin server and loopback load generation to documented disjoint CPU sets.
7. Before every sample, keep the CPUs idle for the declared minimum rest time;
   continue waiting until temperature is at or below the declared ceiling and
   the firmware reports no throttling.
8. Start a fresh server for each sample.
9. Warm for 30 seconds, then measure at least nine 30-second rounds.
10. Rotate implementation order every round.
11. Validate the exact status/body before warmup and after measurement.
12. Sample temperature, frequency, CPU, RSS, threads, descriptors, and context
    switches during every round.
13. For a backend A/B sample, reject a mismatch in protocol, TLS mode, effective
    backend, or `/proc/self/fd` descriptor proof.
14. Close through the public service API, restore the host on every exit path,
    and verify the restored state.

Loopback results are closed-loop connector-attribution evidence. Remote network
capacity needs a separately identified load host and must not share a table
with loopback results.

## Metrics

| Group | Required values |
| --- | --- |
| Correctness | Attempts, successes, HTTP errors, transport errors, exact pre/post body validation |
| Throughput | Requests/second and response bytes/second |
| Latency | p50, p90, p95, p99, p99.9, maximum |
| CPU | Server and load-generator user/system time; cycles/instructions where available |
| Memory | Steady and peak RSS; allocation evidence with its tool/scope |
| Scheduling | Threads, context switches, migrations and wakeups where available |
| Kernel/resources | Descriptors, syscalls and retransmits where applicable |
| Lifecycle | Startup-to-ready and graceful-close latency |
| Stability | Temperature, frequency, throttle flags, thread/fd deltas |

Throughput and p99 are reported together. Median is the primary summary; all
rounds, minimum, maximum, and median absolute deviation remain available.

## Pressure And Lifecycle

Each CoAkka lane also gets a bounded overload campaign:

- set deliberately small connection/active-handler/stream limits;
- hold admitted handlers until capacity is full;
- submit an exact amount of additional work;
- verify admitted, refused, completed, cancelled, and terminal counts;
- prove queue or retained state never exceeds its declared capacity;
- inspect health/monitoring using the methods that connector actually exposes;
- release pressure and prove later work succeeds;
- begin close at each lifecycle phase and verify no resource growth remains.

Disconnect, stalled response, handler timeout, cancellation, and shutdown under
pressure are required. The complete runtime surface in every supported connector
exposes bounded aggregates and a cursor-based event channel. A convenience
builder is described only by the smaller operational view it actually projects.

## Result Rules

- Never use a run that does not prove `application_path=normal` for the CoAkka
  language lane.
- Never reuse historical build-tree numbers as current package results.
- Publish each direct/framework pair separately and only from matched rounds.
- Publish each platform-default/`io_uring` pair separately and never mix its
  HTTP/2 TLS result with HTTP/1.1 framework rows.
- Do not add an external comparator to native C/C++ reference rows.
- Do not publish requests/second without p99, CPU, memory, source, and host
  evidence.
- State whether response bytes are prebuilt or serialized inside the measured
  handler.
- Reject an `io_uring` result without effective-backend and descriptor proof.
- Preserve failed and throttled rounds and explain every exclusion.
- Stop after qualification if identity or order-of-magnitude sanity fails;
  diagnose the product path before spending a full campaign.

## Publication Tables

### Environment

| Field | Value |
| --- | --- |
| Measurement date | Pending accepted application-path campaign |
| Host/OS/kernel | Pending |
| Governor/frequency | Pending |
| Cooling/temperature | Pending |
| Load topology | Pending |
| Tool/package versions | Pending |
| Evidence checksum | Pending |

### Native Reference

| Language | Workload | req/s | p99 | CPU | peak RSS | Notes |
| --- | --- | ---: | ---: | ---: | ---: | --- |
| Pending | | | | | | |

### Application Pair

| Ecosystem | Pair | Workload | Comparator req/s | CoAkka req/s | Paired delta | Comparator p99 | CoAkka p99 | Notes |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | --- |
| Pending | | | | | | | | |

### I/O Backend Pair

| Ecosystem | Workload | Connections x streams | Platform req/s | `io_uring` req/s | Paired delta | Platform p99 | `io_uring` p99 | CPU/RSS notes |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | --- |
| Pending | | | | | | | | |

### Pressure

| Lane | Capacity | Peak state | Admitted | Refused | Lost terminal | Recovery | Close |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| Pending | | | | | | | |

## Open Gate

There is currently no accepted product benchmark for publication. Any private
qualification run is non-publishable. Publication opens only after corrected
application packages, retained source, matching-host execution, raw evidence,
effective-backend and descriptor proof for every `io_uring` lane, restored host
state, and independently reviewable checksums all agree.
