# CoAkka HTTP Runtime for Node.js and Bun

![CoAkka](https://raw.githubusercontent.com/phuong-tran/coakka-samples/a539dfcd7dc8d93c5b1c61b1324899af13d47ca6/docs/common/assets/brand/coakka-logo.png)

CoAkka HTTP Runtime is an embeddable HTTP server and client for JavaScript
and TypeScript on Node.js and Bun. Use it for HTTP APIs, streaming
responses, Server-Sent Events (SSE), WebSockets and calls to upstream
services. Your handlers run in JavaScript or TypeScript. A shared native
core handles HTTP, routing and connection lifecycle on supported Linux,
macOS and Windows targets.

The CoAkka ecosystem provides polyglot tools for distributed applications,
HTTP services and logging. CoAkka HTTP Runtime is the HTTP product and can
be adopted independently.

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md)

## Install 2.2.1

Use the package for your runtime in your application directory.

| Runtime | Requirement | Install |
| --- | --- | --- |
| Node.js | 22 or newer | `npm install coakka-http-runtime-node@2.2.1` |
| Bun | 1.2.22 or newer | `bun add coakka-http-runtime-bun@2.2.1` |

Each package bundles the native libraries for macOS ARM64, Linux x64 and
ARM64, and Windows x64 and ARM64. The package selects the binary for the
runtime's architecture when you import it. On Windows, qualification
covered Bun 1.3.14 on x64 and Bun 1.4.2 on ARM64.

## Basic server

Save this as `server.mjs` and run `node server.mjs`. For Bun, import from
`coakka-http-runtime-bun` and run `bun server.mjs`. Use the same package
name in the examples that follow.

```js
import { Builder, Response } from "coakka-http-runtime-node";

const service = new Builder()
  .listen("127.0.0.1", 8080)
  .get("/hello/{name}", (request) => {
    const name = Buffer.from(request.pathParameters[0].encodedValue).toString();
    return Response.text(`Hello, ${name}!`);
  })
  .start();

console.log(`Listening on http://127.0.0.1:${service.port}`);

process.once("SIGINT", async () => {
  await service.close();
});
```

Open `http://127.0.0.1:8080/hello/Ada`. The server returns HTTP 200 with
`Hello, Ada!`. Press Ctrl+C to stop it.

If the import fails, check the architecture reported by `node -p process.arch`
or `bun -e 'console.log(process.arch)'` and install the package again in your
application directory. Keep the installed package intact when deploying.

## Choose an HTTP version

The first example uses plain HTTP/1.1. For HTTP/2 or HTTP/3 on its own,
replace `.listen(...)` with `.listener(options)`: set `protocol` to
`ListenerProtocol.HTTP_2` (TLS over TCP) or `ListenerProtocol.HTTP_3`
(QUIC over UDP), set `security: TransportSecurity.TLS`, and provide the
certificate and key. Import both enums from the package you installed.
The [complete sample](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/server.mjs) runs either mode.

To serve all 3 versions on the same port, generate the
[local TLS files](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/README.md#prepare-local-tls-files) and save this
as `multi.mjs` next to the `http-test-tls` directory. Use the Bun package
name and `bun multi.mjs` on Bun; use `node multi.mjs` on Node.js.

```js
import { resolve } from "node:path";
import { Builder, Response } from "coakka-http-runtime-node";

const service = new Builder()
  .multiProtocolListener({
    host: "127.0.0.1",
    port: 8443,
    credentialId: "local-example",
    credentialGeneration: 1n,
    certificateChainFile: resolve("http-test-tls/server.pem"),
    privateKeyFile: resolve("http-test-tls/server.key"),
  })
  .get("/hello", () => Response.text("hello"))
  .start();

try {
  console.log(service.listenerInfo());
  await new Promise((stop) => process.once("SIGINT", stop));
} finally {
  await service.close();
}
```

`credentialId` names the TLS identity; `credentialGeneration` identifies
its initial revision. Both file paths must be readable by the server.
Listener V1 enables a fixed profile, so you do not pass a protocol array.
Run the [3 curl probes](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md) against the same URL and check
for `200 1.1`, `200 2` and `200 3`. For remote clients, open TCP **and**
UDP on that port. Use a standalone listener when the application needs
streaming, WebSockets, mTLS or certificate reload.

[HTTP/1.1](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http1.md) · [HTTP/2](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http2.md) · [HTTP/3](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http3.md) · [All 3 on one port](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md)

## API reference

Handlers can return a response or a promise. Header lookup is case-insensitive.
Iteration follows declaration order, and `Headers.getAll()` returns duplicate
values.

- [Node.js and Bun integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/typescript/integration.md)
- [Feature recipes and coverage](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/reference/FEATURES.md)
- [URL grammar and glossary](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/reference/glossary.md)
- [Request bodies](#request-bodies)
- [Streaming responses and SSE](#streaming-responses-and-sse)
- [WebSocket](#websocket)
- [Static and application-selected files](#static-and-application-selected-files)
- [Outbound requests](#outbound-requests)
- [Route updates](#route-updates)
- [Configuration and runtime observations](#configuration-and-runtime-observations)
- [Monitoring and lifecycle](#monitoring-and-lifecycle)
- [Advanced runtime surface](#advanced-runtime-surface)
- [io_uring selection](#io_uring-selection)

## io_uring selection

`io_uring` is opt-in and disabled by default:

```js
const service = new Builder()
  .ioUring(true)
  .get("/ready", () => Response.empty())
  .start();

console.log(service.runtimeInfo());
```

The native runtime checks whether the listener and platform support this
option. It uses the platform default when the option is unavailable.
`runtimeInfo()` reports `ioUringRequested`, `ioUringProbed`, `ioUringEffective`,
and `fallbackReason`.

## Request bodies

For small bodies, use a buffered route:

```js
const service = new Builder()
  .post("/echo", (request) => Response.bytes(request.bytes()))
  .start();
```

Large or incremental bodies use a stream route. Events for concurrent requests
carry distinct generation-checked IDs.

Every admitted stream receives one `dispose` notification when its handler scope
retires, including cancellation before `end`. Release application-owned state
there synchronously and return `undefined`. This notification has no body
sequence and does not assert HTTP success; do not infer transport outcomes from
cleanup or add timers that guess whether a request still exists.

```js
const receivedBytes = new Map();
const key = (event) => `${event.id.slot}:${event.id.generation}`;

const service = new Builder()
  .postStream("/upload", (event) => {
    const id = key(event);
    if (event.kind === "dispose") {
      receivedBytes.delete(id);
    } else if (event.kind === "start") {
      receivedBytes.set(id, 0);
    } else if (event.kind === "data") {
      receivedBytes.set(id, receivedBytes.get(id) + event.data.length);
    } else if (event.kind === "end") {
      return Response.text(String(receivedBytes.get(id)));
    }
    return undefined;
  })
  .start();
```

Only the `end` event returns a response. This example keeps a byte count
per admitted upload, so it never accumulates the body in memory. One handler admission remains reserved for the whole
stream scope, including asynchronous response work. `dispose` runs after that
work settles; a throwing cleanup is reported as a handler diagnostic, never a
second response. Its typed cause distinguishes `end`, `cancelled`, `terminal`,
`handler-failed`, and `service-stopped`. A successful runtime stop reports
`service-stopped`. A refused stop retains the live scope for retry.

## Streaming responses and SSE

```js
import { Builder, StreamingResponse, sse } from "coakka-http-runtime-node";

const service = new Builder()
  .get("/download", () => new StreamingResponse(async (writer) => {
    await writer.write("first\n");
    await writer.write("second\n");
  }), { response: "stream" })
  .get("/events", () => sse([
    { event: "state", id: "1", retry: 1000n, data: "ready" },
  ]), { response: "sse" })
  .start();
```

The stream writer copies each submitted chunk. When the bounded native queue is
full, it waits for a writable event up to the configured response-write timeout.

## WebSocket

```js
import { Builder, websocket } from "coakka-http-runtime-node";

const service = new Builder()
  .get("/socket", () => websocket({
    open(session) {
      session.send("ready");
    },
    message(session, message) {
      if (message.type === "text") session.send(message.data);
    },
  }), { response: "websocket" })
  .start();
```

Callbacks for each session execute in order. Callback errors are retained in the
service's bounded diagnostic queue and can be pulled with `pollError()`.

## Static and application-selected files

Static trees are immutable construction-time declarations:

```js
const service = new Builder()
  .staticMount({ urlPrefix: "/assets", rootPath: "/srv/app/assets" })
  .start();
```

Application-selected files use an explicit authority and remain confined below
its root:

```js
import { Builder, FileResponse } from "coakka-http-runtime-node";

const service = new Builder()
  .fileAuthority({
    id: 1,
    rootPath: "/srv/downloads",
    maxActiveFiles: 16,
    maxFileBytes: 8 * 1024 * 1024,
  })
  .get("/download", () => new FileResponse(1, "/manual.pdf"), {
    response: "file",
  })
  .start();
```

Filesystem traversal and file reads do not run on the JavaScript or HTTP event
loop.

## Outbound requests

Configure logical targets when you build the service, then select them by
name when submitting requests. This example sends `GET /hello/Ada` to the basic server above. Keep it
running on port 8080 and save the following as `client.mjs`.

```js
import { Builder, OutboundReason, TransportSecurity } from "coakka-http-runtime-node";

const service = new Builder()
  .outboundTarget({
    name: "hello",
    generation: 1,
    endpoints: [{
      nodeId: "local",
      connectHost: "127.0.0.1",
      connectPort: 8080,
      httpAuthority: "127.0.0.1:8080",
      security: TransportSecurity.PLAINTEXT,
    }],
  })
  .start();

try {
  const call = service.outboundSubmit({
    logicalTarget: "hello",
    method: "GET",
    target: "/hello/Ada",
  });
  const completion = await service.takeOutbound(3000);
  if (!completion) throw new Error("No outbound result within three seconds");
  if (completion.call.slot !== call.slot || completion.call.generation !== call.generation ||
      completion.reason !== OutboundReason.RESPONSE) {
    throw new Error(`Unexpected outbound result: ${completion.reason}`);
  }
  console.log(completion.responseStatus, Buffer.from(completion.body).toString());
} finally {
  await service.close();
}
```

Run the client with the same runtime as the server. It prints
`200 Hello, Ada!`. `takeOutbound()` polls without blocking the JavaScript event loop and returns a
copied terminal value or `null` on timeout.

## Route updates

On the shared V1 listener above, replace the callback for the existing
route with `service.swapHandler("GET", "/hello", () => Response.text("new"))`.
A request already inside its callback finishes there. The next dispatch
uses the new callback, across all 3 protocols. The listener and its
port stay open.

For a standalone listener, use the generation-checked rebind API instead.
In this fragment, route ID `1` and activation ID `1` refer to the first
route in the basic server above; use the captured IDs and revisions for
the route you actually want to change. Prepare a handler before publishing
its binding ID. Dispatch uses the binding
captured by the request, so an in-flight request never jumps to a newer handler:

```js
service.prepareHandler(2, () => Response.text("new"));
const snapshot = service.routeSnapshot();
const outcome = service.rebindHandler({
  activationId: 1,
  expectedRouteGeneration: snapshot.routeGeneration,
  routeId: 1,
  expectedBindingRevision: snapshot.routes[0].bindingRevision,
  newHandlerBindingId: 2,
});
```

Use `publishRoutes()` only for structural changes such as adding or removing a
route or changing its method, path, or body policy. Both operations use expected
generation values and return the complete effective outcome.

## Configuration and runtime observations

`Builder.compression({ mode: CompressionMode.GZIP })` enables bounded buffered
compression; import `CompressionMode` from `coakka-http-runtime-node`. Omission or
`CompressionMode.DISABLED` leaves it disabled. Identity response streams remain
usable with buffered compression enabled; this does not promise streaming GZIP.
Optional byte/workspace bounds and encoding effort are documented in the
installed `CompressionOptions` fields and validated by the runtime.

`service.routes` and `service.routeSnapshot()` both pull the runtime's coherent route
generation and route/binding revisions. Neither reports a local declaration
cache or invents state when native inspection fails. Use captured route IDs
and expected revisions for control operations.

`OutboundReason`, `OutboundPhase`, `OutboundRetry` and `OutboundCertainty` name
runtime-issued outcome fields. Unknown numeric observations remain unchanged;
application retry safety and budgets are not inferred from status or text.
The [TypeScript sample repository](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/typescript)
has runnable recipes for monitoring, cancellation, deadlines, compression,
and outbound TLS/mTLS. The sample runner uses the SDK and package versions
listed in its own guide.

## Monitoring and lifecycle

Monitoring is disabled unless storage and an initial policy are declared with
`Builder.monitor()`. Health and route snapshots remain pullable independently.
Monitor signals are hints; callers always pull immutable truth with
`monitorConfig()`, `monitorSnapshot()`, or `monitorRead()`.

`close()` is idempotent from the application perspective. It stops admission
and keeps the sole event reader alive until every admitted request has reached
a terminal state. It then interrupts the reader, runs bounded native shutdown,
and releases native state. The reader owns exactly one scheduled Node handle at
a time and cancels that handle with the matching timer API. A failed bounded
drain retains the service so a later `close()` can retry safely. Applications
should still stop creating new work before closing the service.

## Advanced runtime surface

`createRuntime()` exposes the complete low-level pull API for integrations that
need to own event dispatch themselves. Each successful take operation already
returns JavaScript-owned copies, and each lane permits one reader. Do not overlap
blocking reads on the same lane.

[HTTP Runtime documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime)
