# CoAkka HTTP Runtime 2.2.1

Go, Node.js, Bun, Python and JVM 2.2.1 use Core generation 2.2, native SDK
2.2.0 and C ABI 10. C and C++ applications use SDK 2.2.0 directly.
[Versions and SDK compatibility](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/docs/versions.md)

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md)

## Core 2.2

- Listener V1 adds a fixed HTTPS profile for HTTP/1.1, HTTP/2 and HTTP/3
  on the same port. It serves buffered request/reply routes and opens
  both TCP and UDP. Existing ABI 10 entry points continue to support
  standalone listeners.
- The standalone HTTP/3 listener wakes when a handler completes a pending
  response. The client can receive that response without sending another
  request to wake the server.
- CoAkka HTTP Runtime Test adds loopback fixtures that check the version
  negotiated over HTTP/1.1, HTTP/2, HTTP/3 and the shared V1 port.
- CoAkka HTTP Runtime Inspect 2.2.0 was qualified against the Core 2.2
  standalone inspection workflow on macOS and Linux ARM64.

## Connectors 2.2.1

- Go and Python use the qualified Core 2.2.0 SDK. Their existing listener
  APIs select HTTP/1.1, HTTP/2 or HTTP/3 individually.
- Node.js and Bun add `multiProtocolListener()`, `listenerInfo()` and
  `swapHandler()` for listener V1. A request already inside its callback
  keeps that callback. Later dispatches use the replacement callback.
  Existing standalone listeners retain generation-checked rebind.
- Java and Kotlin share the JVM connector and its standalone HTTP/1.1,
  HTTP/2 and HTTP/3 listener API. Text responses reuse the
  UTF-8 bytes created by the text factory. The byte-array response API
  retains its defensive copy of caller-owned arrays.

## Install or upgrade

Use the 2.2.1 packages for
[Go](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/go/README.md),
[Node.js and Bun](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/javascript/README.md),
[Python](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/python/README.md), or
[Java and Kotlin](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/jvm/README.md).
Install SDK 2.2.0 when linking native or Go applications. JVM applications
use the JAR and native ZIP classifier from the same 2.2.1 release.

| Host | Published package | Listener options in this release |
| --- | --- | --- |
| C / C++ | [SDK 2.2.0 and SHA-256 checksums](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.2.1) | Standalone and same port |
| Node.js | [`coakka-http-runtime-node`](https://www.npmjs.com/package/coakka-http-runtime-node/v/2.2.1) | Standalone and same port |
| Bun | [`coakka-http-runtime-bun`](https://www.npmjs.com/package/coakka-http-runtime-bun/v/2.2.1) | Standalone and same port |
| Python | [`coakka-http-runtime-python`](https://pypi.org/project/coakka-http-runtime-python/2.2.1/) | Standalone |
| Java / Kotlin | [`io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.2.1`](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/coakka-http-runtime-jvm/2.2.1) | Standalone |
| Go | [`github.com/phuong-tran/coakka-http-runtime-go/v2@v2.2.1`](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.2.1) | Standalone |

The SDK archives cover Linux x86-64 and ARM64, Windows x86-64 and ARM64,
and macOS ARM64. Go uses the same SDK archives. The Node.js, Bun, Python
and JVM guides list their platform packages.

Try the single-protocol and same-port examples with curl.
[HTTP version exercises](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime/guides/http-versions)

## Verification limit

Bun passed installed-package and protocol checks. On WSL2, ThreadSanitizer
reported a race in a standalone Bun HTTP test before the connector loaded.
The Bun TSan result remains inconclusive.
