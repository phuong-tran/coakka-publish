# CoAkka HTTP Runtime And Go net/http

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and the Go `1.27`
standard-library documentation current on the comparison date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Mental Model](#mental-model)
- [Operations](#operations)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

```go
mux := http.NewServeMux()
mux.HandleFunc("GET /api/hello", func(w http.ResponseWriter, r *http.Request) {
    _, _ = io.WriteString(w, "Hello from Go")
})

server := &http.Server{
    Addr:              ":3000",
    Handler:           mux,
    ReadHeaderTimeout: 5 * time.Second,
}
log.Fatal(server.ListenAndServe())
```

`net/http` supplies the standard Go server, client, handler, multiplexer,
streaming, file, timeout, and shutdown building blocks.

## The CoAkka Shape

```go
service, err := coakkahttp.NewBuilder().
    Listen("127.0.0.1", 3000).
    Get("/api/hello", func(*coakkahttp.Request) (coakkahttp.Response, error) {
        return coakkahttp.Text(200, "Hello from CoAkka")
    }).
    Start()
if err != nil {
    return err
}
defer service.Close()
```

## Mental Model

| Go `net/http` | CoAkka HTTP Runtime |
| --- | --- |
| `ServeMux` patterns | Builder method/path routes |
| `http.Handler` | CoAkka handler function |
| `ResponseWriter` | Returned response value |
| `http.Server` | CoAkka `Service` |
| `Server.Shutdown(ctx)` | Bounded, idempotent `Service.Close()` |
| `FileServer` and `ServeFile` | Static mounts and application-selected file responses |

Both surfaces remain recognizably Go. CoAkka's language connector stays in
the Go host and adds a frozen route set, response values, finite admission,
health, inspection, diagnostics, and service lifecycle.

## Operations

With `net/http`, applications compose server timeouts, middleware, concurrency
limits, metrics, health, and downstream bounds from the standard library and
their chosen packages.

CoAkka places finite connection, active-handler, body, stream, session, and
diagnostic capacity around the Go service. Go applications still own and bound
their own goroutines and downstream work.

Static frontend delivery, SPA fallback, streaming, SSE, WebSocket, rebind,
health, inspection, and the bounded monitor channel remain on the complete
CoAkka runtime API in the same package.

## Choose By Responsibility

Choose `net/http` for a Go-only service that wants the standard library and its
large compatible ecosystem.

Choose CoAkka when Go is one part of a polyglot system that should share HTTP
pressure, health, monitoring, and lifecycle behavior with other languages.

## Official References

- [Go net/http package](https://pkg.go.dev/net/http)
- [Go net/http server documentation](https://pkg.go.dev/net/http#Server)
- [CoAkka Go guide](../../go/README.md)
- [CoAkka Frontend And Backend](../frontend-and-backend.md)
