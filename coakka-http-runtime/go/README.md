# CoAkka HTTP Runtime for Go

Status: GitHub release `1.0.0`. Go module proxy mirror not yet available.

The Go API uses `Request`, `Response`, `Handler`, `Service`, `Client`, stream,
server-event and `WebSocketSession` concepts. Release `1.0.0` includes
buffered and streaming bodies, server events, WebSocket sessions, static and
SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and multipart
values, route updates, bounded inspection, cancellation and finite shutdown.

The exact release artifact is under
[releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-cec8e11](releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-cec8e11/manifest.json).
The suffix identifies the exact Core and connector source snapshots; the
package version remains `1.0.0`. One deterministic source archive carries
target-selected Core route payloads for macOS arm64, Linux arm64/x64 and
Windows arm64/x64. Every target passed the same installed real-socket
capability suite, plus integrity, architecture and native-surface checks. The
x64 Linux and Windows executions used emulation and are not physical-host
performance evidence.

These files are the GitHub-hosted release. Additional dynamic, fault, stress
and soak coverage remains planned; Go module proxy publication is a separate
future distribution step.
