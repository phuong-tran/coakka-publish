# CoAkka HTTP Runtime for JavaScript

Status: GitHub release `1.0.0`. npm mirror not yet available.

The Node and Bun API uses `Request`, `Response`, `StreamResponse`,
`EventResponse`, `WebSocketSession`, `Service` and `Client`. Release `1.0.0`
includes buffered and streaming bodies, server events, WebSocket sessions,
static and SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and
multipart values, route updates, bounded inspection, cancellation and finite
shutdown.

The exact release artifact is under
[releases/1.0.0](releases/1.0.0/manifest.json). One deterministic tarball
contains macOS arm64, Linux arm64/x64 and Windows arm64/x64 payloads. Every
target passed the same installed real-socket capability suite on Node and Bun,
plus integrity, architecture, export and dependency checks. The x64 Linux and
Windows executions used emulation and are not physical-host performance
evidence.

These files are the GitHub-hosted release. Additional dynamic, race, fault,
stress and soak coverage remains planned; npm publication is a separate future
distribution step.
