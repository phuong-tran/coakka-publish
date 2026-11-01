# CoAkka HTTP Runtime Packages

[Choose your language and archive](#download-version-100) · [After the first request](#after-the-first-request) · [Documentation](../docs/coakka-http-runtime/README.md) · [Examples](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/README.md) · [All products](../README.md)

**HTTP services in your language. One shared native core.**

CoAkka HTTP Runtime is a **polyglot, cross-platform HTTP server and client**
backed by a **shared native HTTP core**. Build APIs, serve files or stream
responses through familiar, native-backed language APIs.

A Python upload service and a Go API can use the same HTTP core while keeping
their own handlers. Each application runs its own instance and owns its
business rules. HTTP Runtime handles the HTTP work around those handlers.

HTTP Runtime is independent of CoAkka Runtime, which connects named
application work across processes. Read the
[HTTP introduction](../docs/coakka-http-runtime/explanation/introduction.md)
for the fuller story.

## Why a shared HTTP core

A Go API and a Python upload service can use the same HTTP core through their
own language APIs. Common fixes and improvements can be delivered through
updated language packages. Teams can learn one product's HTTP behavior across
services while keeping application handlers natural to each language.

HTTP Runtime gives the ecosystem an HTTP server/client surface for browser and
HTTP integrations. Runtime remains the product for named application work; an
HTTP handler can call it when needed. Each product can be used on its own.

## Package-manager releases in preparation

Distribution through **npm**, **Maven Central**, **Go modules** and **PyPI** is
planned. The currently available installation path uses the 1.0.0 archives and
matching examples below. The [package ledger](../docs/common/current-packages.md)
records what has actually been released.

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

The [optional Inspect application](inspect/README.md) is distributed separately
for macOS ARM64 and Linux ARM64.

## After the first request

Use the [HTTP documentation](../docs/coakka-http-runtime/README.md) for feature
guides, configuration, deployment and observation. Read the
[product introduction](../docs/coakka-http-runtime/explanation/introduction.md)
for why the shared HTTP contract exists and the language cost tradeoffs.

[Choose another product](../README.md).
