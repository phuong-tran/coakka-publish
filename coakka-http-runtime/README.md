# CoAkka HTTP Runtime Packages

HTTP Runtime runs HTTP services and sends outbound requests from C, C++, Go,
Kotlin, Java, Python, Node.js and Bun. Each service runs its own instance.
Application handlers use their language API while the native runtime handles
HTTP delivery, limits and shutdown.

[HTTP documentation](../docs/coakka-http-runtime/README.md) · [Pi 5 benchmark](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md)

**[coakka-http-runtime-test](runtime-test/)** — read and run the public C and C++
tests against the SDK you install. See how the tests check that requests
stay with the right service, resources are released, and failures are handled.

## Version 2.0.1

[Run your first request](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/registry-basic/README.md) · [Native C and C++ SDK](native/releases/2.0.1/)

| Host | HTTP Runtime 2.0.1 |
| --- | --- |
| Node.js | [`coakka-http-runtime-node`](https://www.npmjs.com/package/coakka-http-runtime-node/v/2.0.1) |
| Bun | [`coakka-http-runtime-bun`](https://www.npmjs.com/package/coakka-http-runtime-bun/v/2.0.1) |
| Python | [`coakka-http-runtime-python`](https://pypi.org/project/coakka-http-runtime-python/2.0.1/) |
| JVM | [`io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.0.1`](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/coakka-http-runtime-jvm/2.0.1) |
| Go | [`github.com/phuong-tran/coakka-http-runtime-go/v2@v2.0.1`](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.0.1) |

Read `2.0.1` as Core generation `2.0` and connector revision `1`. A connector fix advances its own last number and keeps the same native SDK. A Core update changes the first two numbers, and each connector starts again at revision `1` when it adopts that Core. [Versions and SDK compatibility](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/docs/versions.md).

## CoAkka HTTP Runtime Inspect

Inspect can browse a service’s published routes, try requests and export
OpenAPI. Its packages target macOS ARM64 and Linux ARM64. The service must
enable inspection before the companion can connect.
[Walkthrough](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/inspect/walkthrough.md) · [Integration status](https://github.com/phuong-tran/coakka-publish/blob/main/docs/coakka-http-runtime/inspect/metadata-and-openapi.md#current-package-integration)

## Download version 1.0.0

These checksum-pinned archives support the corresponding historical samples.
Use the headers and libraries that match the sample you are building.

| Language | Package directory | First example |
| --- | --- | --- |
| C / C++ | [Native archives](native/candidates/2026-10-08-r3/) | [C](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/c) / [C++](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/cpp) |
| Go | [Go archives](go/candidates/2026-10-08-r3/) | [Go service](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/go) |
| Kotlin / Java | [JVM archives](jvm/candidates/2026-10-08-r3/) | [JVM service](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/kotlin) |
| Node.js / Bun / TypeScript | [Shared JavaScript archives](javascript/candidates/2026-10-08-r3/) | [TypeScript service](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/typescript) |
| Python | [Python archives](python/candidates/2026-10-08-r3/) | [Python service](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/python) |

Each language directory contains five platform archives and `SHA256SUMS`.
Check the selected archive against that directory's checksums. The sample's
language guide explains prerequisites and loading; no private checkout is needed.

The [optional Inspect application](inspect/README.md) is distributed separately
for macOS ARM64 and Linux ARM64.

## After the first request

Use the [HTTP documentation](../docs/coakka-http-runtime/README.md) for feature
guides, configuration, deployment and observation. Read the
[product introduction](../docs/coakka-http-runtime/explanation/introduction.md)
for why the shared HTTP contract exists and the language cost tradeoffs.

[Choose another product](../README.md).
