# CoAkka HTTP Runtime for JavaScript

Status: complete capability development draft. Registry publication is not
open.

The Node and Bun API uses `Request`, `Response`, `StreamResponse`,
`EventResponse`, `WebSocketSession`, `Service` and `Client`. The current draft
includes buffered and streaming bodies, server events, WebSocket sessions,
static and SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and
multipart values, route updates, bounded inspection, cancellation and finite
shutdown.

The exact review candidate is under
[0.1.0-private.0](0.1.0-private.0/manifest.json). One deterministic tarball
contains macOS arm64, Linux arm64/x64 and Windows arm64/x64 payloads. Every
target passed the same installed real-socket capability suite on Node and Bun,
plus integrity, architecture, export and dependency checks. The x64 Linux and
Windows executions used emulation and are not physical-host performance
evidence.

These files are GitHub-level evaluation artifacts. Release-sized dynamic,
race, fault, stress and soak evidence remains open, so there is no npm
coordinate or installation snippet in this draft.
