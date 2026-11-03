# CoAkka HTTP Runtime Packages

[Choose your language and archive](#download-version-100) · [After the first request](#after-the-first-request) · [HTTP Inspect](#coakka-http-runtime-inspect) · [Documentation](../docs/coakka-http-runtime/README.md) · [Examples](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/README.md) · [All products](../README.md)

**HTTP services in your language. One shared native core.**

CoAkka HTTP Runtime provides **polyglot, cross-platform HTTP services and
outbound HTTP requests** backed by a **shared native HTTP core**. Build APIs,
serve files or stream responses through familiar language APIs. Each application
runs its own instance and owns its handlers and business rules.

HTTP Runtime is independent of CoAkka Runtime, which connects named
application work across processes. Read the
[HTTP introduction](../docs/coakka-http-runtime/explanation/introduction.md)
for the fuller story.

## Why a shared HTTP core

A Go API and a Python upload service can use the same HTTP core through their
own language APIs. Common fixes and improvements can be delivered through
updated language packages. Teams can learn one product's HTTP behavior across
services while keeping application handlers natural to each language.

Outbound requests belong to HTTP Runtime's documented package scope. The
planned standalone **CoAkka HTTP Client** is a separate future product; read
[current client scope and roadmap](../docs/coakka-http-runtime/explanation/introduction.md#outbound-http-and-the-planned-http-client).
Runtime remains the product for named application work; an HTTP handler can
call it when needed.

## CoAkka HTTP Runtime Inspect

**Swagger-like API exploration for an inspection-enabled HTTP service.**
Browse the route descriptions a service publishes, try HTTP requests in a
browser and export an **OpenAPI 3.0.3** document. Inspect provides its own UI;
the exported document can also be used by OpenAPI tools such as Swagger UI.

Current Inspect archives target **macOS ARM64** and **Linux ARM64**. Service
inspection must be explicitly enabled, and language-connector APIs for declaring
route/schema metadata are still being completed. Follow the current integration
status before adding inspection to a language example.

[Explore Inspect and OpenAPI](../docs/coakka-http-runtime/inspect/metadata-and-openapi.md) ·
[Current integration status](../docs/coakka-http-runtime/inspect/metadata-and-openapi.md#current-package-integration) ·
[Inspect packages and setup](inspect/README.md).

## Version 1.1.2

| Host | HTTP Runtime 1.1.2 |
| --- | --- |
| Node.js | [`coakka-http-runtime-node`](https://www.npmjs.com/package/coakka-http-runtime-node/v/1.1.2) |
| Bun | [`coakka-http-runtime-bun`](https://www.npmjs.com/package/coakka-http-runtime-bun/v/1.1.2) |
| Python | [`coakka-http-runtime-python`](https://pypi.org/project/coakka-http-runtime-python/1.1.2/) |
| JVM | [`io.github.phuong-tran.coakka:coakka-http-runtime-jvm:1.1.2`](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/coakka-http-runtime-jvm/1.1.2) |
| Go | [`github.com/phuong-tran/coakka-http-runtime-go@v1.1.2`](https://pkg.go.dev/github.com/phuong-tran/coakka-http-runtime-go@v1.1.2) |

[Native C and C++ SDK archives](native/releases/1.1.2/) · [Raspberry Pi 5 benchmark](docs/benchmark-results-rpi5.md)

## Download version 1.0.0

These checksum-pinned repository archives are available here. They are not
npm, Maven Central, PyPI or Go registry releases. Use the matching sample and
archive generation together; older native layouts have different APIs.

| Language | Package directory | Setup and first request |
| --- | --- | --- |
| C / C++ | [Native archives](native/candidates/2026-10-08-r3/) | [C](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/c/integration.md) / [C++](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/cpp/integration.md) |
| Go | [Go archives](go/candidates/2026-10-08-r3/) | [Go service](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/go/integration.md) |
| Kotlin / Java | [JVM archives](jvm/candidates/2026-10-08-r3/) | [JVM service](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/kotlin/integration.md) |
| Node.js / Bun / TypeScript | [Shared JavaScript archives](javascript/candidates/2026-10-08-r3/) | [TypeScript service](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/typescript/integration.md) |
| Python | [Python archives](python/candidates/2026-10-08-r3/) | [Python service](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/python/integration.md) |

Each language directory contains five platform archives and `SHA256SUMS`.
Check the selected archive against that directory's checksums. Each guide gives
prerequisites, archive setup, a runnable service,
its first request and shutdown steps. You only need that language's tools.

The [optional HTTP Runtime Inspect application](inspect/README.md) is distributed separately
for macOS ARM64 and Linux ARM64.

## After the first request

Use the [HTTP documentation](../docs/coakka-http-runtime/README.md) for feature
guides, configuration, deployment and observation. Read the
[product introduction](../docs/coakka-http-runtime/explanation/introduction.md)
for why the shared HTTP contract exists and the language cost tradeoffs.

[Choose another product](../README.md).
