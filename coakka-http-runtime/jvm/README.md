# CoAkka HTTP Runtime for JVM

Status: GitHub release `1.0.0`. Maven Central mirror not yet available.

The Java and Kotlin API is shaped around `HttpRequest`, `HttpResponse`,
`HttpHandler`, `AsyncHttpHandler`, `HttpService` and `HttpClient`. Release
`1.0.0` includes buffered and streaming bodies, server events, WebSocket
sessions, static and SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2,
form and multipart values, route updates, bounded inspection, cancellation and
finite shutdown.

The exact release artifacts are under
[releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-cec8e11](releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-cec8e11/manifest.json)
for macOS arm64, Linux arm64, Linux x86_64, Windows arm64 and Windows x86_64.
The suffix identifies the exact Core and connector source snapshots; the
package version remains `1.0.0`. Every target passed the unit and API suite,
the installed real-socket capability suite, native architecture and export
inspection, dependency checks, extracted-content vocabulary checks, an
isolated Java consumer, and a clean reproducibility rebuild.

These files are the GitHub-hosted release. Additional sanitizer, race, fault,
stress and soak coverage remains planned; Maven Central publication is a
separate future distribution step.
