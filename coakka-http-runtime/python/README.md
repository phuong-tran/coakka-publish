# CoAkka HTTP Runtime for Python

Status: complete capability development draft. Registry publication is not
open.

The Python API uses `Request`, `Response`, `StreamResponse`, `EventResponse`,
`WebSocketSession`, `Service` and `HttpClient`. The current draft includes
buffered and streaming bodies, server events, WebSocket sessions, static and
SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and multipart
values, route updates, bounded inspection, cancellation and finite shutdown.

The exact review candidates are under
[0.1.0-private.0](0.1.0-private.0/manifest.json) for macOS arm64, Linux arm64,
Linux x86_64, Windows arm64 and Windows x86_64. Every target passed the same
installed real-socket capability suite, wheel integrity and embedded Core
identity checks. The x86_64 Linux and Windows executions used emulation and
are not physical-host performance evidence.

These files are GitHub-level evaluation artifacts. Python 3.11 coverage and
release-sized sanitizer, race, fault, stress and soak evidence remain open,
so there is no PyPI coordinate or installation snippet in this draft.
