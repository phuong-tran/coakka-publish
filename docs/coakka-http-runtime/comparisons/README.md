# Platform Comparisons

These pages help developers translate a familiar programming model into CoAkka
HTTP Runtime. They are not feature scores, benchmark results, or claims that
different products solve exactly the same problem.

**Comparison date: 2026-09-13.** CoAkka statements describe private release
`1.0.0`. Other-platform statements are based on the official documentation
linked by each page as it appeared on that date.

## Contents

- [Choose Your Starting Point](#choose-your-starting-point)
- [What Every Page Compares](#what-every-page-compares)
- [How To Read The Code](#how-to-read-the-code)
- [Benchmark Boundary](#benchmark-boundary)

## Choose Your Starting Point

| You use today | Start here |
| --- | --- |
| Node.js built-in HTTP | [CoAkka And Node.js](nodejs.md) |
| `Bun.serve` | [CoAkka And Bun](bun.md) |
| Spring Boot controllers | [CoAkka And Spring Boot](spring-boot.md) |
| Netty channels and pipelines | [CoAkka And Netty](netty.md) |
| Tomcat and Servlets | [CoAkka And Tomcat](tomcat.md) |
| Jetty server and handlers | [CoAkka And Jetty](jetty.md) |
| Go `net/http` | [CoAkka And Go net/http](go-net-http.md) |
| Chi | [CoAkka And Chi](chi.md) |
| Gin | [CoAkka And Gin](gin.md) |
| FastAPI with Uvicorn | [CoAkka And FastAPI/Uvicorn](fastapi-uvicorn.md) |

The [positioning snapshot](../comparison-2026-09-13.md) keeps the compact
cross-platform table. These pages provide the code and context behind it.

## What Every Page Compares

1. The smallest familiar route.
2. The equivalent CoAkka route-and-handler shape.
3. What owns HTTP resources and lifecycle.
4. Where queues, concurrency, and pressure are controlled.
5. How health and monitoring are exposed.
6. How frontend assets, streaming, SSE, and WebSocket fit.
7. What CoAkka intentionally leaves to addons.
8. When the familiar platform is the more direct choice.

## How To Read The Code

The snippets compare developer intent, not byte-for-byte API compatibility.
CoAkka is not a drop-in replacement for a platform's request, response,
middleware, Servlet, or channel types.

The CoAkka snippets use the ordinary builder/service shipped for that language.
Static frontend serving, streaming, handler changes, health, and full monitoring
remain available through the complete runtime surface in the same package.

Framework-style experiences remain valid above CoAkka. For example, a future
Spring-like addon can provide annotations and dependency injection while the
connector retains the shared HTTP contract. The current release
does not claim that such an addon already ships.

## Benchmark Boundary

Architecture and API shape do not prove throughput, latency, or memory use.
Performance comparisons belong to the
[Raspberry Pi 5 benchmark protocol](../benchmark-rpi5.md), which requires the
same workload, host, toolchain, concurrency, connection reuse, run order, and
resource reporting. Every CoAkka language lane must prove normal application-path identity;
release results remain pending. Linux `io_uring` uses a separate same-language
HTTP/2 TLS A/B and never appears as a hidden option in the HTTP/1.1 framework
comparison.
