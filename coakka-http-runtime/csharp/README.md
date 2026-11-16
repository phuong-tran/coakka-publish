# CoAkka HTTP Runtime for C#

CoAkka HTTP Runtime is an embeddable HTTP server and client in the CoAkka ecosystem. The connector runs your application handler and uses Core for HTTP delivery, limits and lifecycle. Connector 2.3.1 uses Core SDK 2.3.0 (C ABI 10).

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [HTTP documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran)

## Install

The package `coakka-http-runtime-csharp` targets .NET 8. You can use it from a
compatible newer .NET application. NuGet selects the native binaries for
your operating system and architecture.

```sh
dotnet add package coakka-http-runtime-csharp --version 2.3.1
```

## Choose a listener

The [first-request and shared listener projects](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/csharp) serve
`GET /hello`. Start with `http1` for a plain loopback request. For `multi`,
generate the sample TLS files and use `server.pem` and `server.key`.

```csharp
var tls = new TlsOptions("sample", 1, "server.pem", "server.key");
var listener = HttpListenerOptions.MultiProtocolListener(
    tls, bindAddress: "127.0.0.1", port: 8443);
```

The shared listener serves buffered requests and responses over HTTP/1.1,
HTTP/2 and HTTP/3 on one TLS port. Run the [three curl probes](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md)
against the same URL to check the negotiated version. Start a standalone
listener to use response streaming, SSE, WebSocket or streamed uploads.

`HttpService.Take` returns application-owned request data. Use the binding
ID to dispatch a handler and respond using the request's exchange. Begin
drain and check its outcome before stopping and disposing the service.
See the [sample launch commands](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners#c) for the full example.
