# CoAkka HTTP Runtime And Node.js

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and the Node.js
HTTP documentation current on that date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Mental Model](#mental-model)
- [Operational Difference](#operational-difference)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

Node.js exposes a low-level request/response server through `node:http`:

```javascript
import * as http from "node:http";

const server = http.createServer((request, response) => {
  if (request.method === "GET" && request.url === "/api/hello") {
    response.writeHead(200, { "content-type": "text/plain" });
    response.end("Hello from Node.js");
    return;
  }
  response.writeHead(404).end();
});

server.listen(3000);
```

The application inspects the request, chooses a route, and writes the response.

## The CoAkka Shape

```javascript
import { Builder, Response } from "@coakka/http";

const service = await new Builder()
  .listen("127.0.0.1", 3000)
  .get("/api/hello", () => Response.text("Hello from CoAkka"))
  .start();

process.once("SIGINT", async () => service.close());
```

The handler remains a normal JavaScript function. The language connector
adds routing, bounds, and lifecycle while keeping the handler in the Node.js
App Host. The complete runtime surface adds health, liveness, inspection, and
monitor control.

## Mental Model

| Node.js HTTP | CoAkka HTTP Runtime |
| --- | --- |
| `http.createServer(listener)` | `new Builder()` plus registered handlers |
| Inspect `request.method` and `request.url` | Register method and path directly |
| `ServerResponse` writes | Return a `Response` value |
| `server.listen(port)` | `builder.listen(...).start()` |
| Node.js host owns its server and HTTP objects | CoAkka service owns its HTTP resources; JavaScript owns handlers and application state |
| Application assembles routing and monitoring | CoAkka supplies routes, bounds, health, snapshots, and a bounded monitor channel |

## Operational Difference

Node.js deliberately provides a low-level HTTP API and supports streaming
messages. Application code and its libraries decide how routing, worker
admission, metrics, and broader overload policy are composed.

CoAkka places finite connection, active-handler, body, stream, session, and
diagnostic capacities around the CoAkka service. The complete runtime API exposes
health and snapshots independently of application logging, plus a bounded
monitor event channel with cursor and missed-history accounting.

Put blocking CPU or I/O work in the application's normal bounded worker/task
facility so the Node.js event loop remains responsive.

## Choose By Responsibility

Choose Node.js built-in HTTP when the application wants direct control of the
Node.js HTTP objects or is already composed around Node streams and middleware.

Choose CoAkka when the same operational HTTP behavior must also serve JVM,
Python, Go, or native applications, or when bounded pressure and a shared
monitoring contract should be present without rebuilding them per language.

For static frontend plus API routing, see
[Frontend And Backend](../frontend-and-backend.md).

## Official References

- [Node.js HTTP API](https://nodejs.org/api/http.html)
- [Node.js streams and backpressure](https://nodejs.org/api/stream.html#buffering)
- [CoAkka How It Works](../how-it-works.md)
- [CoAkka Operations](../operations.md)
