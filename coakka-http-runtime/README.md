# CoAkka HTTP Runtime Packages

**Polyglot applications should not require polyglot HTTP infrastructure.**

When a team uses Go, Python and JavaScript services, it can end up maintaining
several ways to handle the same HTTP concerns: request limits, timeouts,
streaming, monitoring and shutdown. CoAkka HTTP Runtime brings that shared
work into one HTTP engine, exposed through familiar APIs for each language.

Each service runs its own instance. Application handlers keep their business
logic and language conventions; the runtime handles HTTP delivery and service
lifecycle. Teams can reuse operational knowledge across services and receive
shared fixes when they adopt an updated package.

Use it to serve APIs, web frontends, files, streams and WebSocket sessions.
It works independently of CoAkka Runtime, which connects named application
work across processes. Read the
[HTTP Runtime introduction](../docs/coakka-http-runtime/explanation/introduction.md)
or [try a service in your language](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/README.md).

## Download version 1.0.0

These checksum-pinned repository archives are available here. They are not
npm, Maven Central, PyPI or Go registry releases. Use the matching sample and
archive generation together; older native layouts have different APIs.

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
