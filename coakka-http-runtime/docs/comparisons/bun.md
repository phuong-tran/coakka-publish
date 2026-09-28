# CoAkka HTTP Runtime And Bun

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and the Bun server
documentation current on that date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Mental Model](#mental-model)
- [What Inspired CoAkka](#what-inspired-coakka)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

`Bun.serve` combines startup, routing, Fetch-style requests, and responses:

```typescript
Bun.serve({
  port: 3000,
  routes: {
    "/api/hello": {
      GET: () => new Response("Hello from Bun"),
    },
  },
});
```

## The CoAkka Shape

```typescript
import { Builder, Response } from "@coakka/http";

const service = await new Builder()
  .listen("127.0.0.1", 3000)
  .get("/api/hello", () => Response.text("Hello from CoAkka"))
  .start();

process.once("SIGINT", async () => service.close());
```

The same CoAkka package can run in Bun or Node.js. The selected App Host owns
application execution and affects measured cost; CoAkka supplies the same HTTP
service contract in both.

## Mental Model

| Bun | CoAkka HTTP Runtime |
| --- | --- |
| `Bun.serve({...})` | `new Builder()...start()` |
| `routes` or `fetch` | Method/path builder functions |
| Web `Request` and `Response` | CoAkka request and response values |
| Bun runtime owns `Bun.serve` execution | CoAkka owns its service; handlers and application state remain in Bun |
| Bun-specific server controls | Shared CoAkka limits, health, monitoring, and lifecycle |

## What Inspired CoAkka

CoAkka intentionally learns from the directness of `Bun.serve`: define routes,
return response values, start the server, and keep small applications small.
That influence is visible in the ordinary CoAkka builder and returned response
values.

The CoAkka surface adds explicit finite capacities and byte budgets, pressure
outcomes, health, liveness, bounded monitor events, cancellation, and finite
shutdown.

Static frontend mounts, SPA fallback, streaming, SSE, and WebSocket are
available through the complete runtime surface in the same package. An addon can
make those features feel even closer to a single `serve({...})` declaration
without changing the runtime underneath.

## Choose By Responsibility

Choose `Bun.serve` when the application is intentionally Bun-native and wants
its Fetch-style APIs, integrated runtime features, and ecosystem.

Choose CoAkka when Bun is one host among several languages or when the service
needs the same pressure, monitoring, and lifecycle contract outside Bun too.

## Official References

- [Bun HTTP server](https://bun.sh/docs/runtime/http/server)
- [`Bun.serve` reference](https://bun.sh/reference/bun/serve)
- [CoAkka JavaScript and TypeScript guide](../../javascript/README.md)
- [CoAkka Frontend And Backend](../frontend-and-backend.md)
