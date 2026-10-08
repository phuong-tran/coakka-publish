# CoAkka HTTP for JavaScript

CoAkka HTTP gives Node.js and Bun applications a JavaScript-native service API
while the native runtime owns listeners, protocol I/O, bounded queues, route
matching, streaming, files, WebSockets, outbound connections, and monitoring.

Five branch packages serve both Node.js and Bun. Download the `.tgz` matching
the runtime process architecture from [candidates/2026-10-08-r2](candidates/2026-10-08-r3/)
and verify [SHA256SUMS](candidates/2026-10-08-r3/SHA256SUMS). Install that local file:

```sh
npm install --ignore-scripts /absolute/path/coakka-http-javascript-1.0.0-candidate-macos-aarch64.tgz
```

The package loads its own native files by fixed relative paths. Keep `native/`
intact; no compiler, postinstall download, runtime extraction or environment
override is needed. npm publication is not part of this branch preparation.

The API baseline is Node.js22 or Bun1.2.22. On Windows use Bun1.4.2 or later,
prefer native ARM64 Bun on Windows ARM64. Version1.4.2 is the tested reference,
not certification of future versions. Earlier Windows x64 Bun builds crashed
in the ARM64 emulation runner, including a standalone test without CoAkka.
Physical Intel/AMD Windows testing remains separate. The archive's release note
preserves the exact tested scope and related issue links without asserting an
upstream fix. For a deliberate development override only,
`COAKKA_HTTP_JAVASCRIPT_ADDON` accepts an absolute regular addon file; invalid
configuration fails instead of silently choosing another library.

## Contents

- [Basic server](#basic-server)
- [io_uring selection](#io_uring-selection)
- [Request bodies](#request-bodies)
- [Streaming responses and SSE](#streaming-responses-and-sse)
- [WebSocket](#websocket)
- [Static and application-selected files](#static-and-application-selected-files)
- [Outbound requests](#outbound-requests)
- [Route updates](#route-updates)
- [Configuration And Core Observations](#configuration-and-core-observations)
- [Monitoring and lifecycle](#monitoring-and-lifecycle)
- [Advanced runtime surface](#advanced-runtime-surface)

## Basic server

```js
import { Builder, Response } from "@coakka/http";

const service = new Builder()
  .listen("127.0.0.1", 8080)
  .get("/hello/{name}", async (request) => {
    const name = Buffer.from(request.pathParameters[0].encodedValue).toString();
    return Response.text(`Hello, ${name}!`);
  })
  .start();

console.log(`Listening on http://127.0.0.1:${service.port}`);

process.once("SIGINT", async () => {
  await service.close();
});
```

Handlers may return a response directly or through a promise. Header lookup is
case-insensitive and average O(1); declaration order and duplicate fields remain
available through iteration and `Headers.getAll()`.

## io_uring selection

`io_uring` is opt-in and disabled by default:

```js
const service = new Builder()
  .ioUring(true)
  .get("/ready", () => Response.empty())
  .start();

console.log(service.runtimeInfo());
```

JavaScript only sends the boolean preference. Native code decides whether the
listener is eligible, performs the platform probe when appropriate, and falls
back to `epoll` when `io_uring` cannot be used. `runtimeInfo()` reports separate
`ioUringRequested`, `ioUringProbed`, `ioUringEffective`, and `fallbackReason`
fields. The connector never performs an operating-system probe.

## Request bodies

Ordinary routes receive a completely copied request:

```js
const service = new Builder()
  .post("/echo", (request) => Response.bytes(request.bytes()))
  .start();
```

Large or incremental bodies use a stream route. Events for concurrent requests
carry distinct generation-checked IDs:

Every admitted stream receives one `dispose` notification when its handler scope
retires, including cancellation before `end`. Release application-owned state
there synchronously and return `undefined`. This notification has no body
sequence and does not assert HTTP success; do not infer transport outcomes from
cleanup or add timers that guess whether a request still exists.

```js
const chunks = new Map();
const key = (event) => `${event.id.slot}:${event.id.generation}`;

const service = new Builder()
  .postStream("/upload", (event) => {
    const id = key(event);
    if (event.kind === "dispose") {
      chunks.delete(id);
      return undefined;
    }
    if (event.kind === "start") chunks.set(id, []);
    if (event.kind === "data") chunks.get(id).push(event.data);
    if (event.kind === "end") {
      const body = Buffer.concat(chunks.get(id));
      chunks.delete(id);
      return Response.text(String(body.length));
    }
    return undefined;
  })
  .start();
```

Only the `end` event returns a response. The application must keep any
per-request state bounded. One handler admission remains reserved for the whole
stream scope, including asynchronous response work. `dispose` runs after that
work settles; a throwing cleanup is reported as a handler diagnostic, never a
second response. Its typed cause distinguishes `end`, `cancelled`, `terminal`,
`handler-failed`, and `service-stopped`. The last cause follows successful Core
stop; a refused stop retains the live scope for retry.

## Streaming responses and SSE

```js
import { Builder, StreamingResponse, sse } from "@coakka/http";

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
import { Builder, websocket } from "@coakka/http";

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
import { Builder, FileResponse } from "@coakka/http";

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

Logical targets are immutable construction-time snapshots. Calls select a target
by name instead of accepting an arbitrary destination from request data:

```js
const service = new Builder()
  .outboundTarget({
    name: "users",
    generation: 1,
    endpoints: [{
      nodeId: "users-1",
      connectHost: "127.0.0.1",
      connectPort: 9000,
      httpAuthority: "users.internal",
      security: 1,
    }],
  })
  .start();

const call = service.outboundSubmit({
  logicalTarget: "users",
  method: "GET",
  target: "/ready",
});
const completion = await service.takeOutbound(3000);
```

`takeOutbound()` polls without blocking the JavaScript event loop and returns a
copied terminal value or `null` on timeout.

## Route updates

Prepare a handler before publishing its binding ID. Dispatch uses the binding
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

## Configuration And Core Observations

`Builder.compression({ mode: CompressionMode.GZIP })` enables bounded buffered
compression; import `CompressionMode` from `@coakka/http`. Omission or
`CompressionMode.DISABLED` leaves it disabled. Identity response streams remain
usable with buffered compression enabled; this does not promise streaming GZIP.
Optional byte/workspace bounds and encoding effort are documented in the
installed `CompressionOptions` fields and validated by Core.

`service.routes` and `service.routeSnapshot()` both pull Core's coherent route
generation and route/binding revisions. Neither reports a local declaration
cache or invents state when native inspection fails. Use captured route IDs
and expected revisions for control operations.

`OutboundReason`, `OutboundPhase`, `OutboundRetry` and `OutboundCertainty` name
Core-issued outcome fields. Unknown numeric observations remain unchanged;
application retry safety and budgets are not inferred from status or text.
The sibling `coakka-samples` checkout provides the runnable TypeScript recipes
under `coakka-http-runtime/typescript/`: configuration observations, monitor
reload, cancellation, deadlines, compression and native outbound TLS/mTLS.
Use the matching repository package set selected by the sample checksum pins.

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
