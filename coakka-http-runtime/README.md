# CoAkka HTTP Runtime

CoAkka HTTP Runtime is the upcoming CoAkka surface for building HTTP servers
and clients with ordinary request, response, handler, stream and service APIs.
It is a separate product surface from CoAkka Runtime messaging.

## Development Status

This directory is a development preview published at the GitHub repository
level. It is not a released package and is not available from npm, Maven
Central or PyPI. API details, package names and platform coverage may change
before the first release.

The public API uses familiar HTTP vocabulary such as `HttpRequest`,
`HttpResponse` and `HttpHandler`. Applications do not select or learn an
execution mode.

## Language Plans

| Language | Status | Draft |
| --- | --- | --- |
| JVM / Java / Kotlin | Implemented draft; release verification continues | [jvm](jvm/README.md) |
| JavaScript | In development | [javascript](javascript/README.md) |
| Go | In development | [go](go/README.md) |
| Python | In development | [python](python/README.md) |

Each connector is expected to present the same HTTP capabilities in a form
natural to its language: buffered and streaming bodies, asynchronous handlers,
server events, WebSocket sessions, static and SPA delivery, outbound requests,
TLS, supported HTTP protocol selection, form and multipart values, route
updates, bounded inspection, cancellation and finite shutdown.

## Release Plan

1. Complete the language-native API and capability tests for each connector.
2. Rebuild and run matching-host package verification for every advertised
   operating system and architecture.
3. Complete sanitizer, pressure, fault, stress and soak evidence.
4. Add runnable consumer samples only after an exact package is approved.
5. Publish to package registries only through a separate release decision.

Until those gates close, this directory is roadmap and development-status
documentation. It must not be used as evidence that a registry package or
production support claim exists.
