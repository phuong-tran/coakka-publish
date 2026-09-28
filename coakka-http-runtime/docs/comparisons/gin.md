# CoAkka HTTP Runtime And Gin

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and Gin `1.12.0`,
the latest release shown by the official project on the comparison date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Mental Model](#mental-model)
- [Framework Experience](#framework-experience)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

```go
router := gin.Default()
router.GET("/api/hello", func(context *gin.Context) {
    context.String(http.StatusOK, "Hello from Gin")
})

log.Fatal(router.Run(":3000"))
```

Gin combines routing, a request context, middleware, binding, validation,
rendering, and a broader web-framework ecosystem.

## The CoAkka Shape

```go
service, err := coakkahttp.NewBuilder().
    Listen("127.0.0.1", 3000).
    Get("/api/hello", func(*coakkahttp.Request) (coakkahttp.Response, error) {
        return coakkahttp.Text(200, "Hello from CoAkka")
    }).
    Start()
```

## Mental Model

| Gin | CoAkka HTTP Runtime |
| --- | --- |
| `gin.Engine` | CoAkka `Service` |
| `gin.Context` | CoAkka request value and returned response |
| Route groups and middleware | Builder routes today; addon conventions above the connector |
| Binding, validation, rendering | Application or addon responsibility |
| Go HTTP server stack | CoAkka service with Go-owned handlers and values |
| Metrics through middleware/integration | CoAkka health, monitor snapshots/events, plus application telemetry |

## Framework Experience

Gin provides a mature Go framework experience around context, middleware,
binding, validation, and rendering. CoAkka takes a broader product boundary:
its direct builder keeps Go request code small while the operational HTTP
contract also reaches JVM, Python, JavaScript, and native applications.

Bounded resources, explicit pressure and terminal outcomes, health,
monitoring, streaming, static frontend serving, and lifecycle remain
consistent across connectors. A Gin-like addon can add groups, middleware,
binding, or validation without forking that runtime behavior.

## Choose By Responsibility

Choose Gin for a Go web application that benefits from its mature context,
middleware, binding, validation, and rendering experience.

Choose CoAkka for a polyglot system or a service that prioritizes one shared
bounded HTTP and monitoring model. Add higher-level conventions through addons
as they become available.

## Official References

- [Gin documentation](https://gin-gonic.com/en/docs/)
- [Gin routing](https://gin-gonic.com/en/docs/routing/)
- [Gin middleware](https://gin-gonic.com/en/docs/middleware/)
- [Gin releases](https://github.com/gin-gonic/gin/releases)
- [CoAkka How It Works](../how-it-works.md)
