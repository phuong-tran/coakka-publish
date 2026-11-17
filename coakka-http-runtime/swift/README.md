# CoAkka HTTP Runtime for Swift

![CoAkka](https://raw.githubusercontent.com/phuong-tran/coakka-samples/a539dfcd7dc8d93c5b1c61b1324899af13d47ca6/docs/common/assets/brand/coakka-logo.png)

CoAkka HTTP Runtime is an embeddable HTTP server for Swift applications.
Use Swift handlers to serve APIs, accept uploads, stream responses, push
updates with Server-Sent Events (SSE) or WebSockets, and deliver files.
CoAkka HTTP Runtime handles HTTP connections, request limits and shutdown.
Your application defines the routes and the responses.

The CoAkka ecosystem provides polyglot tools for distributed applications,
HTTP services and logging. You can adopt CoAkka HTTP Runtime independently
and run the same HTTP protocols across different application languages.

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [HTTP documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran) · [Ko-fi](https://ko-fi.com/phuongnamtran)

## Read the version

In `2.3.1`, `2.3` is the Core generation and the final `1` is this
connector's revision on that generation. The revision starts at `1` for
each connector. A connector-only update can advance to `2.3.2` while using
the same native SDK and checksums. This release uses Core SDK `2.3.0`
with C ABI 10. [Version and SDK compatibility](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/docs/versions.md)

## Install 2.3.1

Use Swift 6.0 or a compatible newer toolchain. The package can be consumed
with Swift Package Manager (SwiftPM) on the following hosts.

| Platform | Architecture | Notes |
| --- | --- | --- |
| macOS | ARM64 | macOS 14 or newer |
| Linux | x86-64 and ARM64 | glibc 2.28 or newer |
| Windows | x64 and ARM64 | Use a toolchain for the application architecture |

Create a directory for the example. In it, save this as `Package.swift`.

```swift
// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "HelloServer",
    platforms: [.macOS(.v14)],
    dependencies: [
        .package(
            url: "https://github.com/phuong-tran/coakka-http-runtime-swift.git",
            exact: "2.3.1"
        ),
    ],
    targets: [
        .executableTarget(
            name: "HelloServer",
            dependencies: [
                .product(name: "CoAkkaHttp", package: "coakka-http-runtime-swift"),
            ]
        ),
    ]
)
```

The SwiftPM package is `coakka-http-runtime-swift`. Import `CoAkkaHttp` in
the target that uses it.

## First request

Save this as `Sources/HelloServer/main.swift`. It serves one `GET /hello`
request, then drains and stops. It exits if no request arrives within 30
seconds.

```swift
import CoAkkaHttp

enum ServerError: Error {
    case requestTimedOut
    case responseRefused(CoreStatus)
    case drainIncomplete(DrainState, CoreStatus)
}

func serve() throws {
    let helloBinding: UInt64 = 1
    let listener = HttpListener.standalone(protocol: .http11, address: "127.0.0.1", port: 8080)
    let route = HttpRoute(routeID: 1, bindingID: helloBinding, method: "GET", pattern: "/hello")
    let service = try HttpService(listener: listener, routes: [route])
    defer { try? service.close() }

    try service.start()
    print("Listening on http://127.0.0.1:\(try service.port)/hello")

    let body = Array("hello".utf8)
    let headers = [HttpField("content-type", "text/plain; charset=utf-8")]
    let deadline = ContinuousClock.now + .seconds(30)
    while ContinuousClock.now < deadline {
        guard let request = try service.take(timeoutMilliseconds: 250),
              request.kind == .request, request.bindingID == helloBinding else {
            continue
        }

        let status = try service.respond(to: request.exchange, status: 200, body: body, headers: headers)
        if status == .staleExchange {
            continue
        }
        guard status == .ok else {
            throw ServerError.responseRefused(status)
        }

        try service.beginDrain()
        let drain = try service.waitDrained(timeoutMilliseconds: 5000)
        try service.stop()
        guard drain.state == .complete else {
            throw ServerError.drainIncomplete(drain.state, drain.status)
        }
        try service.close()
        return
    }

    throw ServerError.requestTimedOut
}

try serve()
```

On macOS run `swift run HelloServer`. On Linux or Windows, follow
[Build and run on Linux and Windows](#build-and-run-on-linux-and-windows).
When the server prints the listening address, run the following from a
second terminal.

```sh
curl --include --max-time 10 http://127.0.0.1:8080/hello
```

Expect `HTTP/1.1 200` and the body `hello`. Use `curl.exe` in Windows
PowerShell.

Use one `take(timeoutMilliseconds:)` reader per service. Respond with the
exchange from its request event. On the successful path `close()` reports
cleanup failures. The `defer` block covers early exits.

### Build and run on Linux and Windows

From the directory containing `Package.swift`, build with
`swift build -Xswiftc -use-ld=lld` on Linux with LLD installed. On Windows,
run `swift build` from a shell configured for the Swift toolchain. Locate
the executable directory with `swift build --show-bin-path`.

Copy the two libraries from the matching directory in the resolved package
checkout (`.build/checkouts/coakka-http-runtime-swift` by default) into the
executable directory.

| Host | Directory in the checkout | Files |
| --- | --- | --- |
| Linux x86-64 | `Native/linux-x64/` | `libcoakka_http_host.so.1` and `libcoakka_http_managed_host.so` |
| Linux ARM64 | `Native/linux-arm64/` | `libcoakka_http_host.so.1` and `libcoakka_http_managed_host.so` |
| Windows x64 | `Native/win-x64/` | `coakka_http_host.dll` and `coakka_http_managed_host.dll` |
| Windows ARM64 | `Native/win-arm64/` | `coakka_http_host.dll` and `coakka_http_managed_host.dll` |

On Linux run `HelloServer` from the output directory with
`LD_LIBRARY_PATH="$PWD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ./HelloServer`.
On Windows keep the built `CoAkkaHttp.dll` beside `HelloServer.exe` with
the Swift runtime on `PATH`, then run `.\HelloServer.exe`. Use the Linux
instructions when building inside WSL2.

## Choose an HTTP version

| Version | When to use it | Transport |
| --- | --- | --- |
| [HTTP/1.1](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http1.md) | A simple first request or a TCP client using HTTP/1.1 | TCP, with optional TLS |
| [HTTP/2](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http2.md) | Concurrent streams on one HTTPS connection | TCP with TLS |
| [HTTP/3](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http3.md) | Concurrent streams over QUIC when clients can reach the UDP port | UDP with TLS |

For one HTTPS protocol, use `HttpListener.standalone` with `.http2`
or `.http3` and pass a `TlsOptions` value. The complete sample project
runs `http1`, `http2`, `http3` and `multi` modes. [Swift protocol samples](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/swift)

### Serve all 3 protocols on one port

[Generate the local TLS files](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/README.md#prepare-local-tls-files) next to your project. The certificate and
private key configure the listener. The client uses `ca.pem` to verify it.
Replace `Sources/HelloServer/main.swift` with this complete example. The
same program can run a standalone listener by changing its command-line
mode.

```swift
import CoAkkaHttp
import Foundation

enum SampleError: Error {
    case usage(String)
    case responseRefused(CoreStatus)
    case drainFailed(DrainState, CoreStatus)
}

let args = Array(CommandLine.arguments.dropFirst())
guard let mode = args.first, ["http1", "http2", "http3", "multi"].contains(mode),
      args.count == (mode == "http1" ? 1 : 2) else {
    throw SampleError.usage("Usage: Server http1 | Server <http2|http3|multi> <certificate-directory>")
}

let address = "127.0.0.1"
let port: UInt16 = mode == "http1" ? 8080 : 8443
let listener: HttpListener
if mode == "http1" {
    listener = .standalone(protocol: .http11, address: address, port: port)
} else {
    let certificates = URL(fileURLWithPath: args[1], isDirectory: true)
    let tls = TlsOptions(
        credentialID: "sample", generation: 1,
        certificateChainFile: certificates.appendingPathComponent("server.pem").path,
        privateKeyFile: certificates.appendingPathComponent("server.key").path)
    switch mode {
    case "multi": listener = .multiProtocolListener(tls: tls, address: address, port: port)
    case "http2": listener = .standalone(protocol: .http2, address: address, port: port, tls: tls)
    default: listener = .standalone(protocol: .http3, address: address, port: port, tls: tls)
    }
}

let helloBinding: UInt64 = 1
let route = HttpRoute(routeID: 1, bindingID: helloBinding, method: "GET", pattern: "/hello")
let service = try HttpService(listener: listener, routes: [route])
defer { try? service.close() }
try service.start()
let scheme = mode == "http1" ? "http" : "https"
print("Listening on \(scheme)://\(address):\(try service.port)/hello")

var remaining = mode == "multi" ? 3 : 1
let body = Array("hello".utf8)
let headers = [HttpField("content-type", "text/plain; charset=utf-8")]
while remaining > 0 {
    guard let request = try service.take(timeoutMilliseconds: 250),
          request.kind == .request, request.bindingID == helloBinding else { continue }

    let status = try service.respond(to: request.exchange, status: 200, body: body, headers: headers)
    if status == .staleExchange { continue }
    guard status == .ok else { throw SampleError.responseRefused(status) }
    remaining -= 1
}

try service.beginDrain()
let drain = try service.waitDrained(timeoutMilliseconds: 5000)
try service.stop()
print("Drain: \(drain.state) (\(drain.status))")
guard drain.state == .complete else { throw SampleError.drainFailed(drain.state, drain.status) }
try service.close()
```

From the project directory on macOS, run
`swift run HelloServer multi ./http-test-tls`. On Linux or Windows, use
the executable built above with the same arguments.
[Verify all 3 versions with curl](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md) from another terminal. The application serves `hello` at
`https://127.0.0.1:8443/hello`. The three requests should report
`200 1.1`, `200 2` and `200 3`. After the third response, the example begins
drain and reports whether it completed.

Run one listener mode at a time. For remote clients, allow both TCP and
UDP on port 8443. Use a supported standalone listener for streaming, SSE,
WebSockets, mTLS or TLS rotation.

## Build on the example

| Task | Swift API |
| --- | --- |
| Read path and query parameters, headers and a buffered body | `RequestEvent.pathParameters`, `queryParameters`, `headers` and `body` |
| Receive a streaming upload | Set `HttpRoute.bodyMode` to `.streaming` and handle the DATA, trailers and end events from `take` |
| Stream a response | `startResponseStream`, `writeResponseStream` and `finishResponseStream` |
| Push server events | Start with `startServerSentEvents` |
| Serve a WebSocket | Enable `HttpRoute.webSocket` and read connection events with `takeWebSocket` |
| Serve files | Configure `StaticMount` for assets or use `respondFile` with a `FileAuthority` for a file chosen by a handler |
| Read counters | Configure `MonitorOptions` and call `readMonitor` |
| Replace a handler | Use `BufferedHandler.swapHandler` for application dispatch or `rebindHandler` for a supported standalone route |

Use the standalone profile for streaming uploads, response streaming,
SSE, WebSockets and file responses. Uploads carry data from the client
into the application. File responses send server-side files back to a
client. [Feature and profile guide](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime/reference/FEATURES.md#what-you-can-use-in-231)

## Diagnose a failed request

- On startup, inspect `ServiceError.status` and `startupIssue`. If a library
  fails to load, check that the executable and the package libraries use
  the same architecture.
- If `respond` returns `.staleExchange`, the exchange is already retired.
  Drop its reply. Then keep reading events for other requests.
- If a streamed write is paused by capacity, wait for `.responseWritable`
  for that exchange. Stop producing when you receive cancellation or a
  terminal event.
- During shutdown keep serving in-flight exchanges while draining. Inspect
  `waitDrained` before calling `stop`. An `.expired` drain reports that the
  deadline elapsed with work outstanding.
