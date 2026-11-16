# CoAkka HTTP Runtime for Swift

CoAkka HTTP Runtime is an embeddable HTTP server and client in the CoAkka ecosystem. The connector runs your application handler and uses Core for HTTP delivery, limits and lifecycle. Connector 2.3.1 uses Core SDK 2.3.0 (C ABI 10).

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [HTTP documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran)

## Install

The package `coakka-http-runtime-swift` requires Swift tools 6.0.
It provides one SwiftPM package for macOS 14 or later on Apple silicon,
Linux x86_64 and ARM64, and Windows x86_64 and ARM64. See the
[installed-package guide](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners#swift) for the native files needed at launch.

```swift
dependencies: [
    .package(url: "https://github.com/phuong-tran/coakka-http-runtime-swift.git", exact: "2.3.1"),
]
```

Add the product to your executable target's dependencies.

```swift
.product(name: "CoAkkaHttp", package: "coakka-http-runtime-swift")
```

## Choose a listener

The [first-request and shared listener projects](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/swift) serve
`GET /hello`. Start with `http1` for a plain loopback request. For `multi`,
generate the sample TLS files and use `server.pem` and `server.key`.

```swift
let tls = TlsOptions(
    credentialID: "sample", generation: 1,
    certificateChainFile: "server.pem", privateKeyFile: "server.key")
let listener = HttpListener.multiProtocolListener(
    tls: tls, address: "127.0.0.1", port: 8443)
```

The shared listener serves buffered requests and responses over HTTP/1.1,
HTTP/2 and HTTP/3 on one TLS port. Run the [three curl probes](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md)
against the same URL to check the negotiated version. Start a standalone
listener to use response streaming, SSE, WebSocket or streamed uploads.

Use `HttpService.take(timeoutMilliseconds:)` to read requests. Dispatch by
binding ID and respond using the request's exchange. Begin drain and check
its outcome before stopping and closing the service.
