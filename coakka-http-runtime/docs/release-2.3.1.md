# CoAkka HTTP Runtime 2.3.1

Go, Node.js, Bun, Python, JVM, C# and Swift 2.3.1 use Core generation 2.3,
SDK 2.3.0 and C ABI 10. The final `1` is each connector's first revision
on Core 2.3. Use SDK 2.3.0 for C and C++. [How to read versions](versions.md)

- Go, Python, Java and Kotlin add application APIs for HTTP/1.1, HTTP/2 and
  HTTP/3 on the same TLS port. Node.js and Bun keep their existing
  `multiProtocolListener()` API.
- C# adds the NuGet package `CoAkka.Http.Runtime` for .NET 8 and later.
  Swift adds the SwiftPM package `coakka-http-runtime-swift` for Swift 6.0 and
  later. Both have a shared TLS listener and standalone listener examples.
- The shared TLS listener serves buffered requests and responses. Use a
  standalone listener for features such as streaming and WebSocket.
- Core 2.3 fixes the standalone HTTP/2 and HTTP/3 uploads and HTTP/3 response
  streaming and SSE. Standalone HTTP/2 and HTTP/3 WebSocket use Extended CONNECT.
- Install the Java or Kotlin JAR and native ZIP classifier from the same
  connector release. Go uses its versioned module with the matching native SDK.

[Installation and examples](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime) ·
[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product)

[C# installation](../csharp/README.md) · [Swift installation](../swift/README.md) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran) · [Ko-fi](https://ko-fi.com/phuongnamtran)
