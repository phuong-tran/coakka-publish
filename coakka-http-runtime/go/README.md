# CoAkka HTTP for Go

`coakkahttp` is the idiomatic Go connector for CoAkka HTTP Runtime. The module
links directly to the installed host library; it contains no native payload and
does not extract a library at runtime.

Status: development source candidate. A tagged Go module and registry-oriented
package layout are separate release gates.

## Requirements

- Go 1.23 or newer
- cgo and a C11 compiler
- `coakka/http/host.h`
- `libcoakka_http_host` for the current target

For a non-system installation, pass its include and library directories to
cgo and make the library discoverable when running tests:

```sh
CGO_CFLAGS="-I$COAKKA_HTTP_PREFIX/include" \
CGO_LDFLAGS="-L$COAKKA_HTTP_PREFIX/lib" \
go test ./...
```

Use the platform loader setting appropriate for the target during development,
for example `DYLD_LIBRARY_PATH` on macOS or `LD_LIBRARY_PATH` on Linux. A final
package-manager layout is intentionally not defined here.

## Quick start

```go
package main

import (
    "log"

    coakkahttp "github.com/phuong-tran/coakka-http-runtime-go"
)

func main() {
    service, err := coakkahttp.NewBuilder().
        Get("/hello/{name}", func(request *coakkahttp.Request) (coakkahttp.Response, error) {
            name := request.PathParameters[0].EncodedValue
            return coakkahttp.Text(200, "hello "+name)
        }).
        Start()
    if err != nil {
        log.Fatal(err)
    }
    defer func() { _ = service.Close() }()

    port, err := service.Port()
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("listening on http://127.0.0.1:%d", port)
    select {}
}
```

`Builder` supplies a bounded request/reply facade. `Runtime` is the lower-level
pull API for applications that need direct ownership of events and commands.
Both use the same host API and the same native service.

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

Prepare a new handler binding before publishing it. `RebindHandler` changes only
the selected route's binding revision; `PublishRoutes` replaces one complete
immutable structural generation. Admitted requests continue through the
binding they captured. Rejected changes preserve the previous effective state.

Application dispatch uses bounded maps keyed by handler identity. The connector
does not linearly scan handlers on the request path; structural matching and
route identity lookup remain native-owned.

## Shutdown

`Runtime.Close` interrupts readers, waits for active calls, drains and stops the
native service, then destroys it. `Service.Close` additionally joins its fixed
worker set and stream/WebSocket helpers. User handlers must return; Go cannot
forcibly stop arbitrary application code. A timed-out close retains the owner
so cleanup can be retried safely.
