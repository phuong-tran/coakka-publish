# CoAkka HTTP Runtime for JVM

Status: implemented development draft. Registry publication is not open.

The Java and Kotlin API is shaped around `HttpRequest`, `HttpResponse`,
`HttpHandler`, `AsyncHttpHandler`, `HttpService` and `HttpClient`. The current
draft includes buffered and streaming bodies, server events, WebSocket
sessions, static and SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2,
form and multipart values, route updates, bounded inspection, cancellation and
finite shutdown.

Current source verification includes Java 8 API compilation, real socket
lifecycle tests and a macOS arm64 package candidate. Other target builds and
release-sized verification remain open, so there is no Maven Central
coordinate or installation snippet here yet.
