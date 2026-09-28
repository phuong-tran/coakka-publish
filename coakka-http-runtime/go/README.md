# CoAkka HTTP For Go

CoAkka HTTP for Go provides ordinary Go handlers for buffered request/reply and
a complete typed `CoAkka HTTP Runtime` surface for protocols, streaming,
realtime traffic, security, files, outbound calls, live control, and monitoring.

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

The Go module publication gate remains closed. Extract the exact private source
artifact and point a temporary module replacement at it:

```sh
go mod edit -require=github.com/phuong-tran/coakka-http-runtime-go@v0.0.0
go mod edit -replace=github.com/phuong-tran/coakka-http-runtime-go=/absolute/path/to/extracted/package
go mod tidy
```

Remove the local replacement when the public module coordinate is opened.

## Quick Start

```go
service, err := coakkahttp.NewBuilder().
    Listen("127.0.0.1", 8080).
    Get("/hello", func(_ *coakkahttp.Request) (coakkahttp.Response, error) {
        return coakkahttp.Text(200, "Hello from CoAkka")
    }).
    Start()
if err != nil {
    return err
}
defer service.Close()
```

The handler receives a Go-owned request and returns a Go response plus error.
The builder is single-use and freezes its routes and bounds at start.

## Two Useful API Levels

| API | Best fit |
| --- | --- |
| `NewBuilder()` and `Service` | Buffered HTTP request/reply with finite connection, active-handler, body, header, stream, session, and diagnostic bounds |
| `Config` and `Open()` | HTTP/1.1, HTTP/2, HTTP/3 where available, streams, trailers, SSE, WebSocket, files, outbound HTTP, TLS/mTLS, `io_uring`, handler swap, health, liveness, inspection, and monitoring |

Event take methods copy native values into Go storage before returning.
Run one reader for each inbound, WebSocket, outbound-terminal, or monitor-wait
lane, then fan out copied work through application-owned bounded channels.

## Capacity And Backpressure

Finite defaults cover connections, active handlers, bodies, headers, routes,
stream chunks, sessions, diagnostics, and shutdown waits. The complete runtime
surface additionally bounds request/completion queues, outbound work, files,
monitor history, and terminal state. Pressure, limit, timeout, cancellation,
closed state, and application error remain distinguishable.

Goroutines, channels, database pools, retries, and exporter queues created by
the application need their own limits and cancellation.

## Observability And Monitoring

The complete runtime surface exposes `Health()`, `ProbeLiveness()`,
`MonitorConfig()`, `MonitorSnapshot()`, generation-checked
`ApplyMonitorPolicy()`, cursor-based `ReadMonitorEvents()`, `WaitMonitor()`, and
`InterruptMonitor()`.

Monitor storage is bounded and reports overwritten or missed history. It does
not retain HTTP bodies, credentials, cookies, or certificate material. Read
[Observability And Monitoring](../docs/observability-and-monitoring.md) for the
data model and operator loop.

## TLS, mTLS, And Live Handler Changes

`Config.Listeners` accepts protocol, TLS/mTLS mode, credential
identity/generation, certificate chain, private key, and trust roots. Supported
Linux HTTP/2 and HTTP/3 configurations may select `IOUring`.

`runtime.Rebind(...)` switches a stable route to a prepared Go handler binding
under route-generation and binding-revision checks. See
[TLS And mTLS](../docs/tls-and-mtls.md) and
[Live Handler Changes](../docs/handler-swap-and-hot-reload.md).

## Lifecycle

`Service.Close()` is idempotent. `runtime.Close()` rejects new calls, interrupts
owned waiters, waits for finite active calls, drains/stops the runtime, and then
releases the loaded image. Application readers and workers must observe
cancellation and converge before their owner returns.

## Targets And Current Gate

The private source artifact contains target selections for macOS ARM64, Linux
ARM64, Linux x86-64, Windows ARM64, and Windows x86-64. Capability bits from
the loaded target are authoritative.

Public module publication, public sample promotion, and portable performance
claims remain closed until the corrected source artifact, matching-host tests,
documentation, and Raspberry Pi evidence agree.
