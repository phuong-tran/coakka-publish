# CoAkka HTTP Runtime Packages

CoAkka HTTP Runtime runs HTTP services and sends outbound requests from C, C++, Go,
Kotlin, Java, Python, Node.js, Bun, C# and Swift. Each service runs its own instance.
Application handlers use their language API while the native runtime handles
HTTP delivery, limits and shutdown.

[HTTP documentation](../docs/coakka-http-runtime/README.md) · [Pi 5 benchmark](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran) · [Ko-fi](https://ko-fi.com/phuongnamtran)

**[coakka-http-runtime-test](runtime-test/)** — read and run the public C and C++
tests against the SDK you install. See how the tests check that requests
stay with the right service, resources are released, and failures are handled.

## Node.js and Bun 2.3.1 on npm

Node.js and Bun 2.3.1 are available with Core SDK 2.3.0 and ABI 10.

```sh
npm install coakka-http-runtime-node@2.3.1
bun add coakka-http-runtime-bun@2.3.1
```

[Publication and package hashes](../docs/coakka-http-runtime/releases/2026-11-17-node-bun-2.3.1-npm.md) ·
[Node.js and Bun guide](javascript/README.md)

## Python 2.3.1 on PyPI

Python 2.3.1 is available with Core SDK 2.3.0 and ABI 10.
Install `coakka-http-runtime-python==2.3.1`.
[Publication and wheel hashes](../docs/coakka-http-runtime/releases/2026-11-17-python-2.3.1-pypi.md) ·
[Python guide](python/README.md)

## Go 2.3.1 on GitHub

The Go module and five native SDK 2.3.0 archives are available in
[GitHub Release v2.3.1](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.3.1).

```sh
go get github.com/phuong-tran/coakka-http-runtime-go/v2@v2.3.1
```

[Installation and MultiProtocol example](go/README.md) ·
[Changelog](go/CHANGELOG.md) ·
[Publication and SDK hashes](../docs/coakka-http-runtime/releases/2026-11-17-go-2.3.1-github.md)

## Java and Kotlin 2.3.1 on Maven Central

Use `io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.3.1`
and the matching platform's native ZIP classifier.
[JVM guide](jvm/README.md) ·
[Publication and package hashes](../docs/coakka-http-runtime/releases/2026-11-18-jvm-2.3.1-central.md)

## C# 2.3.1 on NuGet

```sh
dotnet add package CoAkka.Http.Runtime --version 2.3.1
```

The package targets .NET 8 and includes all 5 native targets.
[C# guide and MultiProtocol example](csharp/README.md) ·
[Publication and package hashes](../docs/coakka-http-runtime/releases/2026-11-18-csharp-2.3.1-nuget.md)

## Swift 2.3.1 on GitHub

```swift
.package(url: "https://github.com/phuong-tran/coakka-http-runtime-swift.git", exact: "2.3.1")
```

[GitHub Release and checksums](https://github.com/phuong-tran/coakka-http-runtime-swift/releases/tag/v2.3.1) ·
[Swift guide and complete examples](swift/README.md) ·
[Verified publication](../docs/coakka-http-runtime/releases/2026-11-18-swift-2.3.1-github.md)

## Previous coordinated release: connector 2.2.1 and Core SDK 2.2.0

[Release notes](docs/release-2.2.1.md) · [Native SDK downloads and SHA-256 checksums](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.2.1) · [Run an HTTP example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/protocol-listeners/README.md)

| Host | Package |
| --- | --- |
| Node.js | [`coakka-http-runtime-node`](https://www.npmjs.com/package/coakka-http-runtime-node/v/2.2.1) |
| Bun | [`coakka-http-runtime-bun`](https://www.npmjs.com/package/coakka-http-runtime-bun/v/2.2.1) |
| Python | [`coakka-http-runtime-python`](https://pypi.org/project/coakka-http-runtime-python/2.2.1/) |
| Java / Kotlin | [`io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.2.1`](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/coakka-http-runtime-jvm/2.2.1) |
| Go | [`github.com/phuong-tran/coakka-http-runtime-go/v2@v2.2.1`](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.2.1) |

## Previous release: 2.0.1

[Run your first request](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/registry-basic/README.md) · [Native C and C++ SDK downloads and checksums](native/releases/2.0.1/)

| Host | HTTP Runtime 2.0.1 |
| --- | --- |
| Node.js | [`coakka-http-runtime-node`](https://www.npmjs.com/package/coakka-http-runtime-node/v/2.0.1) |
| Bun | [`coakka-http-runtime-bun`](https://www.npmjs.com/package/coakka-http-runtime-bun/v/2.0.1) |
| Python | [`coakka-http-runtime-python`](https://pypi.org/project/coakka-http-runtime-python/2.0.1/) |
| JVM | [`io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.0.1`](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/coakka-http-runtime-jvm/2.0.1) |
| Go | [`github.com/phuong-tran/coakka-http-runtime-go/v2@v2.0.1`](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.0.1) |

Read `2.0.1` as Core generation `2.0` and connector revision `1`. A connector fix advances its own last number and keeps the same native SDK. A Core update changes the first two numbers, and each connector starts again at revision `1` when it adopts that Core. [Versions and SDK compatibility](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/docs/versions.md).

## Connector 2.3.1

Go, Node.js, Bun, Python, Java/Kotlin, C# and Swift 2.3.1 are published
on their respective channels. They use Core SDK 2.3.0 and ABI 10. The final `1` is each connector's first revision on Core 2.3.
See the [release notes](docs/release-2.3.1.md),
[the protocol examples](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md) and
[how to read the version](docs/versions.md).

## CoAkka HTTP Runtime Inspect

Inspect can browse a service’s published routes, try requests and export
OpenAPI. Its packages target macOS ARM64 and Linux ARM64. The service must
enable inspection before the companion can connect.
[Inspect 2.2.0 downloads and checksums](inspect/releases/2.2.0/) · [2.0.1 Node.js walkthrough](inspect/walkthrough.md) · [Integration status](https://github.com/phuong-tran/coakka-publish/blob/main/docs/coakka-http-runtime/inspect/metadata-and-openapi.md#current-package-integration)

## Archived 1.0.0 feature packages

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

[Release 2.2.1 and its changelog](docs/release-2.2.1.md) describe Core SDK 2.2.0 and the matching connectors.

[Java and Kotlin 2.3.1 are available on Maven Central](../docs/coakka-http-runtime/releases/2026-11-18-jvm-2.3.1-central.md), with SDK 2.3.0 and ABI 10.
