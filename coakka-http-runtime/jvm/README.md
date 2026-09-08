# CoAkka HTTP Runtime for JVM

Status: complete capability development draft. Registry publication is not
open.

The Java and Kotlin API is shaped around `HttpRequest`, `HttpResponse`,
`HttpHandler`, `AsyncHttpHandler`, `HttpService` and `HttpClient`. The current
draft includes buffered and streaming bodies, server events, WebSocket
sessions, static and SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2,
form and multipart values, route updates, bounded inspection, cancellation and
finite shutdown.

The exact review candidates are under
[0.1.0-private.0](0.1.0-private.0/manifest.json) for macOS arm64, Linux arm64,
Linux x86_64, Windows arm64 and Windows x86_64. Every target passed the unit
and API suite, the installed real-socket capability suite, native architecture
and export inspection, dependency checks, extracted-content vocabulary checks,
an isolated Java consumer, and a clean reproducibility rebuild.

These files are GitHub-level evaluation artifacts. Release-sized sanitizer,
race, fault, stress and soak evidence remains open, so there is no Maven
Central coordinate or installation snippet in this draft.
