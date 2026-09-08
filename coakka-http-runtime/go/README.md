# CoAkka HTTP Runtime for Go

Status: complete capability development draft. Module publication is not
open.

The Go API uses `Request`, `Response`, `Handler`, `Service`, `Client`, stream,
server-event and `WebSocketSession` concepts. The current draft includes
buffered and streaming bodies, server events, WebSocket sessions, static and
SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and multipart
values, route updates, bounded inspection, cancellation and finite shutdown.

The exact review candidate is under
[0.1.0-private.0](0.1.0-private.0/manifest.json). One deterministic source
archive carries target-selected Core route payloads for macOS arm64, Linux
arm64/x64 and Windows arm64/x64. Every target passed the same installed
real-socket capability suite, plus integrity, architecture and native-surface
checks. The x64 Linux and Windows executions used emulation and are not
physical-host performance evidence.

These files are GitHub-level evaluation artifacts. Release-sized dynamic,
fault, stress and soak evidence remains open, so there is no public Go module
version or installation command in this draft.
