# CoAkka HTTP For Python

`coakka_http` gives Python a direct async service API and the complete typed
`CoAkka HTTP Runtime` surface. Handlers, requests, responses, application
state, and async scheduling stay Python-owned; the runtime owns its HTTP resources,
protocol state, bounds, monitoring, and shutdown.

## Contents

- [Install](#install)
- [Quick Start](#quick-start)
- [Two Useful API Levels](#two-useful-api-levels)
- [Capacity And Backpressure](#capacity-and-backpressure)
- [Observability And Monitoring](#observability-and-monitoring)
- [TLS, mTLS, And Live Handler Changes](#tls-mtls-and-live-handler-changes)
- [Lifecycle](#lifecycle)
- [Targets And Current Gate](#targets-and-current-gate)

## Install

The `1.0.0` wheel train remains private. Install the exact wheel for the target
machine from the staged release directory:

```sh
python -m pip install "./coakka_http-1.0.0-py3-none-<platform>.whl"
```

The wheel includes the matching `CoAkka HTTP Runtime` image. No compiler
or separate native runtime installation is required. Direct loading from a
wheel ZIP is rejected; install it with a wheel-aware tool.

## Quick Start

```python
import asyncio

from coakka_http import Builder, Request, Response


async def hello(_request: Request) -> Response:
    return Response.text("Hello from CoAkka")


async def main() -> None:
    service = await (
        Builder()
        .listen("127.0.0.1", 8080)
        .get("/hello", hello)
        .start()
    )
    try:
        print(f"http://127.0.0.1:{service.port}")
        await service.wait()
    finally:
        await service.close()


asyncio.run(main())
```

The builder is single-use. A handler is an ordinary async Python function that
receives a Python-owned `Request` and returns a `Response`.

## Two Useful API Levels

| API | Best fit |
| --- | --- |
| `Builder` and `Service` | Async buffered request/reply with finite connection, active-handler, header, body, response, and backlog bounds |
| `Configuration` advanced service | HTTP/1.1, HTTP/2, HTTP/3 where available, streams, trailers, SSE, WebSocket, files, outbound HTTP, TLS/mTLS, `io_uring`, handler swap, health, inspection, and monitor control |

The complete runtime surface uses typed event leases. Each lease is a context
manager and must be released exactly once. Copy values before handing work to
another thread or queue. Only one reader owns each inbound, WebSocket,
outbound-terminal, or monitor-wait lane.

## Capacity And Backpressure

The async service bounds connections, active handlers, backlog, request target,
header count and bytes, request body bytes, response header count and bytes,
response body bytes, and route count. Active-handler pressure receives a stable
`503`; invalid or oversized input receives a specific HTTP refusal; handler
failure receives `500`. `service.snapshot()` exposes running, dispatched,
rejected, failed, pending, and configured active-handler capacity.

The runtime additionally bounds connections, routes, headers, chunks, streams,
sessions, outbound work, monitor history, file work, and terminal state.
Application-created tasks, database pools, retries, and exporter queues remain
the application's responsibility.

## Observability And Monitoring

The async service exposes its bounded `service.snapshot()` counters.
The complete runtime surface exposes:

- `health()` and `probe_liveness()`;
- `monitor_config()` and `monitor_snapshot()`;
- generation-checked `monitor_apply()`;
- cursor-based `monitor_read()` with `missed_events`;
- one finite `monitor_wait()` lane and `monitor_interrupt()` for shutdown.

Monitoring is disabled by default and never retains HTTP payloads or
credentials. See [Observability And Monitoring](../docs/observability-and-monitoring.md)
for a complete Python configuration and operator loop.

## TLS, mTLS, And Live Handler Changes

`Configuration.add_listener(Listener(...))` accepts a complete listener
declaration, including HTTP protocol, TLS or mutual TLS, credential generation,
certificate chain, private key, and client trust roots. The complete runtime
surface also supports generation- and revision-checked `rebind()` for switching
an existing route to a prepared Python handler binding.

See [TLS And mTLS](../docs/tls-and-mtls.md) for runnable Python listener
examples and [Live Handler Changes](../docs/handler-swap-and-hot-reload.md) for
the safe activation and drain workflow.

## Lifecycle

`await Service.close()` stops admission, waits on a finite monotonic deadline
for active handlers, closes the owned server, and releases the runtime. Close is
serialized and idempotent. `await Service.wait()` lets a process owner wait for
terminal server state without taking over application signal policy.

For direct runtime use, call `drain()` to stop admission while the inbound reader
continues completing accepted work. After active work converges, interrupt and
join every owned reader, release all leases, call `stop()`, then `close()`.
The runtime fails closed if destruction overlaps a live call or lease.

## Targets And Current Gate

Private wheels exist for macOS ARM64, Linux ARM64, Linux x86-64, Windows ARM64,
and Windows x86-64. Capability bits from the loaded package are authoritative
for optional protocols and providers on a target.

PyPI publication, public sample promotion, and portable performance claims
remain closed until the corrected package, matching-host tests, docs, samples,
and Raspberry Pi evidence agree.
