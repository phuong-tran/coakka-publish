# CoAkka HTTP Runtime And Chi

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and the official
Chi documentation current on the comparison date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Different Layers](#different-layers)
- [Middleware And Addons](#middleware-and-addons)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

```go
router := chi.NewRouter()
router.Use(middleware.RequestID)
router.Use(middleware.Timeout(30 * time.Second))
router.Get("/api/hello", func(w http.ResponseWriter, r *http.Request) {
    _, _ = io.WriteString(w, "Hello from Chi")
})

log.Fatal(http.ListenAndServe(":3000", router))
```

Chi is an idiomatic, composable router compatible with `net/http` and its
middleware model.

## The CoAkka Shape

```go
service, err := coakkahttp.NewBuilder().
    Listen("127.0.0.1", 3000).
    Get("/api/hello", func(*coakkahttp.Request) (coakkahttp.Response, error) {
        return coakkahttp.Text(200, "Hello from CoAkka")
    }).
    Start()
```

## Different Layers

| Chi | CoAkka HTTP Runtime |
| --- | --- |
| Router and middleware on `net/http` | CoAkka Go service and route contract |
| Implements `http.Handler` | Uses CoAkka request and response values |
| Go ecosystem composition | Shared behavior across eight language surfaces |
| Middleware supplies timeout, throttle, logging, and more | Connector supplies bounds, pressure, health, and lifecycle; addons supply conventions |

Chi and CoAkka do not occupy the same layer. Chi deliberately builds on the Go
standard server. CoAkka supplies its own cross-language runtime contract.

## Middleware And Addons

Chi's middleware ecosystem is a major strength. Its official middleware
includes timeout, heartbeat, request ID, compression, and concurrency throttle;
compatible community middleware composes through `http.Handler`.

CoAkka `1.0.0` does not claim drop-in `net/http` middleware compatibility.
Equivalent application conventions belong in Go addons above the connector.
CoAkka pressure, health, and monitoring stay below that addon boundary.

## Choose By Responsibility

Choose Chi when the service is Go-native and should remain fully compatible
with `net/http` handlers and middleware.

Choose CoAkka when routing is only one part of a shared polyglot HTTP,
backpressure, monitoring, and lifecycle model.

## Official References

- [Chi project and router interface](https://github.com/go-chi/chi)
- [Chi middleware](https://pkg.go.dev/github.com/go-chi/chi/v5/middleware)
- [CoAkka App Host And Connectors](../app-host-and-connectors.md)
- [CoAkka Operations](../operations.md)
