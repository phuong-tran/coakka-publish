# CoAkka HTTP Runtime Capabilities

This matrix separates the complete `CoAkka HTTP Runtime` contract from
the smaller convenience builders. Release `1.0.0` remains private, so every
optional row still requires capability detection and matching-host evidence
before a public support claim.

## Contents

- [Status Vocabulary](#status-vocabulary)
- [Complete Runtime Contract](#complete-runtime-contract)
- [Language Projection](#language-projection)
- [Convenience Builders](#convenience-builders)
- [Capacity And Backpressure](#capacity-and-backpressure)
- [Observability And Monitoring](#observability-and-monitoring)
- [Frontend And Backend](#frontend-and-backend)
- [Platform Conditions](#platform-conditions)
- [Publication Gate](#publication-gate)

## Status Vocabulary

| Status | Meaning |
| --- | --- |
| Built in | Part of the shared runtime contract |
| Projected | Exposed as typed values and methods in the named language connector |
| Feature gated | API exists; the loaded target must report the capability |
| Host specific | Same contract with a language-specific execution or ownership shape |
| Publication pending | Implemented private candidate; external release gate remains closed |

## Complete Runtime Contract

| Area | Capability | Current contract |
| --- | --- | --- |
| Inbound | HTTP/1.1 | Built in |
| Inbound | HTTP/2 | Built in, feature gated per target |
| Inbound | HTTP/3 | Built in, feature gated per target |
| Security | TLS and mutual TLS | Built in, file-backed server identity and trust roots, provider feature gated per target |
| Routing | Method/path matching, whole-segment captures, query and headers | Built in |
| Request | Buffered body, streamed body, trailers, cancellation | Built in |
| Response | Buffered body, stream, trailers, writability and write timeout | Built in |
| Encoding | Gzip response policy | Built in, feature gated |
| Realtime | Server-Sent Events and WebSocket | Built in, feature gated |
| Files | Static mounts, SPA fallback, application-authorized files, ranges and validators | Built in, target filesystem support required |
| Outbound | Bounded client, logical targets, pools, DNS, TLS identity/trust, cancel and terminal outcome | Built in, feature gated |
| Live control | Handler-binding swap with generation, revision, and idempotency checks | Built in, feature gated |
| Observability | Health, fresh liveness, typed outcomes and inspection | Built in |
| Monitoring | Bounded aggregates, failures, latency buckets, recent events, cursor/loss accounting, wait/interrupt, live policy | Built in, disabled by default |
| I/O selection | Platform-default backend | Built in |
| I/O selection | Linux `io_uring` | Feature gated for eligible HTTP/2 and HTTP/3 listener configurations |
| Lifecycle | Start, drain, interrupt, stop and idempotent release | Built in with finite configured deadlines |

The feature result from the exact loaded package is authoritative. An exported
method keeps the language API stable; it does not make an unavailable target
provider silently appear.

## Language Projection

| Language | Advanced service entry | Events and copied values | Monitor channel | Handler swap |
| --- | --- | --- | --- | --- |
| C/C++ | `coakka_http_host_service_t` | Explicit leased C values | `coakka_http_host_monitor_read/wait/interrupt` | `coakka_http_host_rebind` |
| Java/Kotlin | `HttpRuntime` | JVM-owned sealed/data values | `readMonitorEvents/waitForMonitor/interruptMonitorWaiter` | `rebind` |
| Python | `Runtime(RuntimeConfig(...))` | Typed context-managed leases | `monitor_read/monitor_wait/monitor_interrupt` | `rebind` |
| JavaScript/TypeScript | `createRuntime()` | JavaScript-owned copied objects | `monitorRead/monitorWait/monitorInterrupt` | `rebind` |
| Go | `Open()` | Go-owned structs and slices | `ReadMonitorEvents/WaitMonitor/InterruptMonitor` | `Rebind` |

All five projections cover the complete runtime capability vocabulary. Their
reader, scheduling, and close idioms intentionally follow the host language.

## Convenience Builders

The convenience layer keeps first use simple. It is not the product's
capability ceiling.

| Language | Builder | Current convenience scope |
| --- | --- | --- |
| C/C++ | `coakka_http_host_*` | Explicit leased events, finite bounds, and explicit lifecycle |
| Java/Kotlin | `ServiceBuilder` | Direct buffered handlers with finite event loops, active-handler admission, and body/header/route bounds |
| Python | `Builder` | Async buffered handlers with finite connection, active-handler, header, body, response, route, and backlog bounds |
| JavaScript/TypeScript | `Builder` | Synchronous or asynchronous handlers on Node.js or Bun with finite active-handler, body, and route bounds plus Promise close |
| Go | `NewBuilder()` | Buffered handlers with finite connection, active-handler, body, header, stream, session, and diagnostic bounds |

Use the builder for normal request/reply. Use the complete runtime owner when the
application needs streaming, realtime sessions, files, outbound calls,
TLS/mTLS control, `io_uring`, handler swap, inspection, or the monitor channel.
An addon can provide annotations, decorators, middleware, dependency injection,
generated routes, or another framework-style experience above either surface.

## Capacity And Backpressure

| Boundary | Required behavior |
| --- | --- |
| Connections and active exchanges | Finite ceiling; excess admission is refused |
| Request dispatch | Fixed workers and/or finite item and retained-byte queues |
| Headers and targets | Count, byte, and path limits checked before application use |
| Request body | Per-request byte ceiling; streaming adds finite chunk budgets |
| Response body and stream | Byte/chunk ceilings; stalled writes wait, cancel, or time out |
| WebSocket | Finite sessions, frames, message bytes, and pending output |
| Static/application files | Confined authority, finite active work and response bytes |
| Outbound calls | Finite registry, pools, in-flight requests, response bytes, and terminals |
| Diagnostics and monitoring | Finite retained errors, events, detail, failure rows, and latency buckets |
| Shutdown | Monotonic finite convergence; live ownership is not freed underneath callers |

Pressure, limit, timeout, cancellation, closed state, application failure, and
service failure remain distinguishable. Application-owned databases,
executors, goroutines/tasks, retries, and exporter queues still need bounds.

## Observability And Monitoring

Every complete language projection exposes health, a fresh-progress liveness
probe, aggregate monitor snapshots, generation-checked live monitor policy,
and a bounded recent-event channel. Cursor pages report overwritten history;
monitor pressure never blocks HTTP traffic or changes an exchange result.

Read [Observability And Monitoring](observability-and-monitoring.md) for the
metric set, data exclusion rules, event flow, and language API names.

## Frontend And Backend

One runtime service can own explicit `/api/*` routes, a built frontend and SPA
fallback, server-event streams, WebSocket sessions, and outbound HTTP. Explicit
application routes win before static fallback. Static roots and
application-file authorities are confined and bounded.

Read [Frontend And Backend](frontend-and-backend.md) for the route order and
deployment shape.

## Platform Conditions

- The private target matrix contains macOS ARM64, Linux ARM64, Linux x86-64,
  Windows ARM64, and Windows x86-64 packages.
- Static and SPA serving has passed on macOS ARM64, Linux x86-64, Windows ARM64,
  and Windows x86-64. Linux ARM64 remains pending on the clean Trixie host;
  applications must still check the loaded capability and use a confined root.
- Explicit `io_uring` is Linux only and currently belongs to eligible HTTP/2
  and HTTP/3 configurations, not HTTP/1.1.
- Protocol, security, outbound, and filesystem support must be checked through
  capability bits and a matching-host test.

## Publication Gate

The source and private candidates implement the capabilities above. Registry
publication, public sample promotion, portable performance claims, and a
production-support statement remain closed until package content, language
surface, docs, source-visible samples, and matching-host evidence all agree.
