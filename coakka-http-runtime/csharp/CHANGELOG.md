# CoAkka HTTP Runtime for C# changelog

## 2.3.1

First public C# connector revision on Core generation **2.3**, native SDK
**2.3.0**, C ABI **10**. The package targets **net8.0** and runs on .NET 8
or compatible newer .NET versions.

- Provide standalone HTTP/1.1, HTTP/2 and HTTP/3 listeners and a
  `MultiProtocolListener` for buffered requests on one TLS port.
- Support buffered and streamed uploads, request and response trailers,
  response streaming, SSE, standalone WebSockets and file delivery.
- Expose outbound request, cancellation and terminal results, monitoring,
  health, route control and supported TLS/mTLS credential reload.
- Include native libraries for macOS ARM64, Linux x64/ARM64 and Windows
  x64/ARM64. WSL2 uses the Linux x64 assets.
- Include complete NuGet installation and standalone/MultiProtocol examples,
  version guidance, ecosystem and benchmark links, GitHub Sponsors and Ko-fi.

In 2.3.1, 2.3 identifies the Core generation and 1 is this connector's
revision. A connector-only update increments the final number while
keeping the qualified native SDK and its checksums unchanged.

[GitHub Sponsors](https://github.com/sponsors/phuong-tran) ·
[Ko-fi](https://ko-fi.com/phuongnamtran)
