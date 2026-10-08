# CoAkka HTTP Runtime Capabilities

This matrix describes the shared `CoAkka HTTP Runtime` contract and its
idiomatic application surfaces. Builders are not limited to buffered HTTP.
Every optional capability still requires feature detection and matching-host
evidence for the selected package and deployment.

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
| Live control | Handler-binding swap and complete route-generation publication, with versioned acceptance/rejection | Built in, feature gated |
| Configuration | CPU/batch/timeout intent and runtime-issued effective runtime information | Inspect the exact public package surface |
| Observability | Health, fresh liveness, typed outcomes and inspection | Built in |
| Browser tooling | Route/schema snapshots, OpenAPI and route try | Optional [HTTP Runtime Inspect](../inspect/README.md); separate macOS ARM64/Linux ARM64 local-development packages |
| Monitoring | Bounded aggregates, failures, latency buckets, recent events, cursor/loss accounting, wait/interrupt, live policy | Built in, disabled by default |
| I/O selection | Platform-default backend | Built in |
| I/O selection | Linux `io_uring` | Feature gated for eligible HTTP/2 and HTTP/3 listener configurations |
| Lifecycle | Start, drain, interrupt, stop and idempotent release | Built in with finite configured deadlines |

The feature result from the exact loaded package is authoritative. An exported
method keeps the language API stable; it does not make an unavailable target
provider silently appear.

## Language Projection

| Host | Normal application surface | Ownership |
| --- | --- | --- |
| C | `coakka_http_server_t`, declared callbacks, installed `coakka/http/http.h` | Callback-scoped borrowed requests; checked stop/destroy |
| C++ | The same public C API wrapped with application RAII | No exception crosses a C callback; retain state on close refusal |
| Kotlin/Java | `ServiceBuilder` | Kotlin implementation, Java-friendly public values and lifecycle |
| Go | `NewBuilder()` | Functions, structs, errors and context-aware application flow |
| Python | `Builder` | Python values and explicit service lifetime |
| Node.js/Bun | `Builder` | Value or Promise-returning buffered handlers; synchronous stream chunk callbacks, asynchronous producers and close |

The native application package uses `CoAkkaHttp::runtime`, not the connector
host API. Advanced public native configuration and event operations also live
in `http.h`; an application never needs the private build-tree runtime header.
Consult each exact package declaration before using a lower-level entrypoint.

## Convenience Builders

The ordinary Go, Kotlin, Python and TypeScript examples already demonstrate
buffered and streaming HTTP, SSE, WebSocket, static/application files, outbound
requests, monitor reads, handler replacement and route publication. Their
security examples configure TLS/mTLS. These features do not inherently require
abandoning the idiomatic service builder for a manual event pump.

The C/C++ samples include buffered callbacks, path/query/header access, files,
request/response streaming, SSE, WebSocket, outbound calls, route publication,
runtime information, monitor control, handler replacement and secure listeners.
The feature index below distinguishes named examples and executed recipes from
remaining failure-path coverage; API availability alone is not test evidence.

CPU budgets, batch settings and timeouts belong to the runtime. Connectors submit
intent and expose the runtime's accepted effective state through runtime information.
Do not infer effective CPU/backend settings from the requested values. Internal
loop tuning is not an application knob in normal language builders.

For runnable source and honest remaining coverage, use the
[feature sample index](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/FEATURES.md)
on `main`. Framework-style authentication,
annotations, dependency injection and business routing policy remain app-host
or addon responsibilities.

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
- Artifact-backed application and TLS/mTLS samples pass on macOS ARM64 and
  Raspberry Pi Linux ARM64. Package qualification for other targets is separate
  from executing these samples. Static/application roots remain confined.
- HTTP/1.1 security sample success does not establish HTTP/2 or HTTP/3 wire
  coverage; check the exact target capability and corresponding protocol evidence.
- Explicit `io_uring` is Linux only and currently belongs to eligible HTTP/2
  and HTTP/3 configurations, not HTTP/1.1.
- Protocol, security, outbound, and filesystem support must be checked through
  capability bits and a matching-host test.

## Publication Gate

The source and private candidates implement the capabilities above. Registry
publication, public sample promotion, portable performance claims, and a
production-support statement remain closed until package content, language
surface, docs, source-visible samples, and matching-host evidence all agree.
