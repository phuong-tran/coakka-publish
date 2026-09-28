# CoAkka HTTP For JavaScript And TypeScript

One ESM package runs on Node.js and Bun, with TypeScript declarations included.
It provides a direct buffered builder and the complete
`CoAkka HTTP Runtime` surface. JavaScript owns handlers and application
state; the runtime owns its HTTP resources, protocol state, limits, monitoring, and
shutdown.

## Contents

- [Install](#install)
- [Quick Start](#quick-start)
- [Node.js And Bun](#nodejs-and-bun)
- [Two Useful API Levels](#two-useful-api-levels)
- [Capacity And Backpressure](#capacity-and-backpressure)
- [Observability And Monitoring](#observability-and-monitoring)
- [TLS, mTLS, And Live Handler Changes](#tls-mtls-and-live-handler-changes)
- [Lifecycle](#lifecycle)
- [Targets And Current Gate](#targets-and-current-gate)

## Install

The current package remains private. Install the exact staged tarball:

```sh
npm install ./coakka-http-1.0.0.tgz
# or
bun add ./coakka-http-1.0.0.tgz
```

The package selects and verifies its matching target image at load time. No
compiler or separately installed native runtime is required.

## Quick Start

```javascript
import { Builder, Response } from "@coakka/http";

const service = await new Builder()
  .listen("127.0.0.1", 8080)
  .concurrency(2)
  .get("/hello", () => Response.text("Hello from CoAkka"))
  .get("/health", () => Response.text("ok"))
  .start();

console.log(`http://127.0.0.1:${service.port}`);
```

Version `1.0.0` buffered handlers are synchronous and must return a `Response`.
Promises are rejected rather than allowed to create ambiguous lifetime or
shutdown behavior.

## Node.js And Bun

Imports and application code stay the same on Node.js and Bun. Benchmark and
support evidence always name the host and version; a result from one host is
not relabeled as evidence for the other.

Blocking work must not run on the main JavaScript event loop. Use the host's
normal Worker/task mechanism and keep its queue, retained bytes, cancellation,
and shutdown bounded.

## Two Useful API Levels

| API | Best fit |
| --- | --- |
| `Builder` and `Service` | Synchronous buffered handlers on the host event loop, finite active-handler/body/route bounds, and Promise close |
| `createCore()` advanced service | HTTP/1.1, HTTP/2, HTTP/3 where available, streaming, SSE, WebSocket, files, outbound HTTP, TLS/mTLS, `io_uring`, handler swap, health, liveness, and monitor control |

Runtime event values are copied into JavaScript before they are returned. Use
zero-timeout polling on the event loop or a dedicated Worker for blocking take
or wait calls. Only one reader owns each inbound, WebSocket, outbound-terminal,
or monitor-wait lane.

## Capacity And Backpressure

Connections, active exchanges, request and response queues, bodies, headers,
chunks, routes, sessions, outbound work, retained diagnostics, monitor events,
and shutdown waits have finite ceilings. Exhausted capacity produces a stable
typed refusal, cancellation, or timeout instead of unbounded accumulation.

Buffered handler failures produce a stable `500`. Applications that need the
complete health, failure aggregates, recent-event history, or operator wait
lane use the typed runtime surface described below.

## Observability And Monitoring

The complete runtime surface exposes `health()`, `probeLiveness()`,
`monitorConfig()`, `monitorSnapshot()`, generation-checked `monitorApply()`,
cursor-based `monitorRead()`, `monitorWait()`, and `monitorInterrupt()`.

The event channel reports overwrite/drop and missed-history truth. It never
retains request bodies, credentials, cookies, or certificate material. Read
[Observability And Monitoring](../docs/observability-and-monitoring.md) for the
collection model and complete language map.

## TLS, mTLS, And Live Handler Changes

`createCore({ listener: ... })` accepts the protocol, TLS/mTLS mode, credential
identity/generation, certificate chain, private key, and trust roots. On
supported Linux HTTP/2 and HTTP/3 configurations it can select `IoBackend.IO_URING`.

`runtime.rebind(...)` atomically changes one stable route to a prepared handler
binding under route-generation and binding-revision checks. See
[TLS And mTLS](../docs/tls-and-mtls.md) and
[Live Handler Changes](../docs/handler-swap-and-hot-reload.md).

## Lifecycle

`service.close()` returns one idempotent Promise that stops admission, drains
callbacks, and releases the service. Importing the package does not take over
process signals:

```javascript
process.once("SIGTERM", async () => {
  await service.close();
});
```

For direct runtime use, call `runtime.drain()` while the event reader continues
completing accepted work. After active work converges, interrupt and join every
Worker or reader, call `runtime.stop()`, then `runtime.close()`. Do not overlap runtime
destruction with another runtime operation.

## Targets And Current Gate

The private package contains verified target selections for macOS ARM64, Linux
ARM64, Linux x86-64, Windows ARM64, and Windows x86-64. Capability bits from
the loaded target are authoritative.

npm publication, public sample promotion, and portable performance claims
remain closed until Node.js and Bun matching-host evidence agrees with the
corrected package and documentation.
