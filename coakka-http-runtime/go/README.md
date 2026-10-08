# CoAkka HTTP for Go

`coakkahttp` is the idiomatic Go connector for CoAkka HTTP Runtime. The module
links directly to the matching library supplied beside the Go module. It does
not extract or download a library at runtime.

Status: five packaged branch candidates, not a tagged module or registry
release. Download from [candidates/2026-10-08-r3](candidates/2026-10-08-r3/) and verify
[SHA256SUMS](candidates/2026-10-08-r3/SHA256SUMS). Each bundle supplies `go/` and
`native/` together, installation instructions and legal material.

## Contents

- [Requirements](#requirements)
- [Quick start](#quick-start)
- [I/O backend selection](#io-backend-selection)
- [Supported service features](#supported-service-features)
- [Response compression](#response-compression)
- [Low-level example](#low-level-example)
- [Monitoring](#monitoring)
- [Route changes](#route-changes)
- [Shutdown](#shutdown)

## Requirements

- Go 1.23 or newer
- cgo and a C11 compiler
- `coakka/http/host.h`
- `libcoakka_http_host` for the current target

From an application module on macOS/Linux, use the extracted bundle:

```sh
export COAKKA_HTTP_PACKAGE=/absolute/path/to/extracted-bundle
go mod edit -require=github.com/phuong-tran/coakka-http-runtime-go@v0.0.0
go mod edit -replace=github.com/phuong-tran/coakka-http-runtime-go="$COAKKA_HTTP_PACKAGE/go"
export CGO_ENABLED=1
export CGO_CFLAGS="-I$COAKKA_HTTP_PACKAGE/native/include"
export CGO_LDFLAGS="-L$COAKKA_HTTP_PACKAGE/native/lib -Wl,-rpath,$COAKKA_HTTP_PACKAGE/native/lib"
go build -trimpath .
```

Use an application-owned library location when deploying; do not retain a
developer path. Windows needs a cgo-capable compiler for the selected process
architecture, `native/include`, `native/lib`, and `native/bin` on the process
`PATH` (or the DLL beside the executable). Follow the included platform guide.

## Quick start

```go
package main

import (
    "context"
    "log"
    "os"
    "os/signal"

    coakkahttp "github.com/phuong-tran/coakka-http-runtime-go"
)

func main() {
    ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt)
    defer stop()
    service, err := coakkahttp.NewBuilder().
        Get("/hello/{name}", func(request *coakkahttp.Request) (coakkahttp.Response, error) {
            name := request.PathParameters[0].EncodedValue
            return coakkahttp.Text(200, "hello "+name)
        }).
        Start()
    if err != nil {
        log.Fatal(err)
    }
    port, err := service.Port()
    if err != nil {
        closeErr := service.Close()
        log.Fatalf("read port: %v; close: %v", err, closeErr)
    }
    log.Printf("listening on http://127.0.0.1:%d", port)
    <-ctx.Done()
    if err := service.Close(); err != nil {
        log.Fatal(err)
    }
}
```

`Builder` supplies a bounded request/reply facade. `Runtime` is the lower-level
pull API for applications that need direct ownership of events and commands.
Both use the same host API and the same native service.

## Response compression

Configure ordinary host-inlined handlers with
`builder.Compression(&coakkahttp.Compression{Mode: coakkahttp.CompressionGZIP, MinimumBodyBytes: 1, GZIPLevel: 6})`
before startup. The builder copies the optional value; `Compression(nil)`
removes an earlier override. Explicit disable uses
`&coakkahttp.Compression{Mode: coakkahttp.CompressionDisabled}` without GZIP-only
tuning fields. Core rejects incompatible settings rather than ignoring them.

Core owns response eligibility, resource ceilings and negotiation. Handlers
return normal application values; do not add another compression layer. The
Go boundary rejects an unknown mode that cannot be represented by the native
boolean. Numeric bounds and capability decisions remain Core-owned. This is
startup configuration, not a live change to existing connections.

The transform applies to eligible buffered responses, not response streams or
SSE. Streams remain uncompressed and bounded; an identity-refusing client
receives HTTP406 before an unencoded stream starts when this policy is enabled.
Application-supplied content encoding remains application-owned.

## I/O backend selection

The default configuration leaves `Config.IOBackend` at zero, which selects the
platform backend. On Linux this is the normal epoll path. To request io_uring:

```go
runtime, err := coakkahttp.Open(coakkahttp.Config{
    IOBackend: coakkahttp.IOUring,
    Routes: []coakkahttp.Route{{
        ID: 1, HandlerBindingID: 1, Method: "GET", Pattern: "/ready",
    }},
})
```

The connector does not inspect the kernel or issue a probe. The native library
owns eligibility, initialization, and fallback. After `Start`, read
`RuntimeInfo()` to distinguish requested and effective backends and to inspect
the typed fallback reason.

## Supported service features

The current Go surface includes:

- HTTP/1.1, HTTP/2, and HTTP/3 listener declarations
- plaintext, TLS, and mutual TLS
- buffered and streamed request bodies, including trailers
- buffered responses, response streams, trailers, and server-sent events
- WebSocket upgrade, receive, send, writable, close, and failure events
- gzip response compression with native defaults for omitted bounds
- immutable static trees, directory index, SPA fallback, and application file
  authorities
- logical outbound targets, TLS trust and client identities, submit, cancel,
  and terminal reads
- health snapshots and bounded active liveness probes
- disabled-by-default monitoring with fixed reservations and live policy apply
- handler rebinding and complete route-table publication with generation and
  revision checks

All counts, queues, bodies, retained bytes, deadlines, and shutdown waits have
construction-time bounds. Zero-valued optional limits select bounded native
defaults.

## Low-level example

```go
runtime, err := coakkahttp.Open(coakkahttp.Config{
    Listeners: []coakkahttp.Listener{{
        ID:          1,
        BindAddress: "127.0.0.1",
        Protocol:    coakkahttp.ProtocolHTTP11,
        Security:    coakkahttp.SecurityPlaintext,
    }},
    Routes: []coakkahttp.Route{{
        ID:               1,
        HandlerBindingID: 1,
        Method:           "POST",
        Pattern:          "/echo",
    }},
})
if err != nil {
    return err
}
defer runtime.Close()

if err = runtime.Start(); err != nil {
    return err
}
event, err := runtime.TakeEvent(250 * time.Millisecond)
if err != nil {
    return err
}
if event.Kind == coakkahttp.EventRequest {
    reply, _ := coakkahttp.Bytes(200, event.Request.Bytes())
    err = runtime.Respond(event.Exchange, reply)
}
```

Use exactly one goroutine for each pull lane. Returned events are Go-owned
copies: the connector releases the native lease before the method returns.

## Monitoring

Monitoring starts disabled. Reserve the resources that may be enabled later:

```go
builder.Monitor(coakkahttp.MonitorOptions{
    Collection:          coakkahttp.MonitorDisabled,
    EventCapacity:       256,
    MaxEventsPerRead:    32,
    AggregateCategories: coakkahttp.MonitorCategoryLifecycle |
        coakkahttp.MonitorCategoryExchange,
    EventCategories: coakkahttp.MonitorCategoryLifecycle,
    SignalReserved:  true,
})
```

`MonitorConfig`, `MonitorSnapshot`, generation-checked `ApplyMonitorPolicy`,
bounded event reads, wait, and interrupt all delegate to the native owner.
Monitor saturation never changes an HTTP exchange result.

## Route changes

`Service.Routes()` and `Runtime.Routes()` return `(RouteSnapshot, error)` from
Core: one complete structural generation, binding-change sequence and bounded
route/binding list. The caller owns the copy; mutation cannot change routing.
Monitoring need not be enabled. Refusal or unresolved control returns no partial
snapshot. This unreleased contract replaces the old local-declaration getter.

Prepare a new handler binding before publishing it. `RebindHandler` changes only
the selected route's binding revision; `PublishRoutes` replaces one complete
immutable structural generation. Admitted requests continue through the
binding they captured. Rejected changes preserve the previous effective state.

Application dispatch uses bounded maps keyed by handler identity. The connector
does not linearly scan handlers on the request path; structural matching and
route identity lookup remain native-owned.

## Shutdown

Outbound terminals preserve typed `OutboundReason`, `OutboundPhase`,
`OutboundRetry` and `OutboundCertainty`, including unknown numbers. Use named
constants: HTTP404/500 can be `OutboundResponse`, while cancellation and deadline
expiry remain distinct causes. `Reason.String()` derives a code without storing
a duplicate name. Retry disposition does not authorize replaying business work;
the application owns idempotency and retry budgets. Never expose operator
diagnostics automatically in an HTTP response.

`Service.Close` stops admission, observes drain outcome and joins Go work before
destruction. Application handlers must return. Inspect
`*CloseError.RuntimeRetained` after refusal and retain the service when cleanup
needs retry. `*DrainError` preserves forced expiry or abort even after cleanup.
Low-level `Runtime.Close` is forced cleanup: graceful low-level use requires
`Drain`, progressing readers and handlers, then `WaitDrained` first. A timed wait
is not proof of completion; an expired drain is not graceful success.
