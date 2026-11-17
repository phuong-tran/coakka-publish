# CoAkka HTTP Runtime for C#

![CoAkka](https://raw.githubusercontent.com/phuong-tran/coakka-samples/a539dfcd7dc8d93c5b1c61b1324899af13d47ca6/docs/common/assets/brand/coakka-logo.png)

CoAkka HTTP Runtime is an embeddable HTTP server and client for .NET
applications. Use C# to serve APIs, accept uploads, stream responses,
push updates with Server-Sent Events (SSE) or WebSockets, deliver files,
and call upstream services. Your application owns the routes and business
logic; CoAkka HTTP Runtime owns HTTP connections, bounded resources and
request lifecycle.

Serve HTTP/1.1, HTTP/2 and HTTP/3 separately, or use `MultiProtocolListener`
for buffered requests on one TLS port. The NuGet package includes the
managed library and matching native libraries for all 5 supported targets.

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

Use .NET 8 or a compatible newer .NET version. The NuGet package targets
`net8.0` and includes the native libraries for the application architecture.

| Platform | Architecture | Notes |
| --- | --- | --- |
| macOS 14 or newer | ARM64 | Use .NET 8 or newer |
| Linux | x86-64 and ARM64 | glibc 2.28 or newer |
| Windows | x64 and ARM64 | Use a toolchain for the application architecture |

Create a console project, then add the package from NuGet.

```sh
dotnet new console --name HelloServer --framework net8.0
cd HelloServer
dotnet add package coakka-http-runtime-csharp --version 2.3.1
```

Install `coakka-http-runtime-csharp` and import `CoAkka.Http`. NuGet selects
native assets for the application runtime. On WSL2, run the Linux .NET SDK;
Windows applications use Windows assets. No separate native SDK download
is required for this package.

## First request

Replace `Program.cs` with this example. It serves one `GET /hello` request,
then drains and stops. It exits if no request arrives within 30 seconds.

```csharp
using System.Diagnostics;
using System.Text;
using CoAkka.Http;

const ulong helloBinding = 1;
var listener = HttpListenerOptions.Standalone(
    HttpProtocol.Http11, bindAddress: "127.0.0.1", port: 8080);
var route = new HttpRoute(RouteId: 1, BindingId: helloBinding, Method: "GET", Pattern: "/hello");

using var service = new HttpService(listener, [route]);
service.Start();
Console.WriteLine($"Listening on http://127.0.0.1:{service.Port}/hello");

byte[] body = Encoding.UTF8.GetBytes("hello");
HttpField[] headers = [new("content-type", "text/plain; charset=utf-8")];
var elapsed = Stopwatch.StartNew();
while (elapsed.Elapsed < TimeSpan.FromSeconds(30))
{
    var request = service.Take(timeoutMilliseconds: 250);
    if (request is not { Kind: EventKind.Request, BindingId: helloBinding })
        continue;

    var status = service.Respond(request.Exchange, 200, body, headers);
    if (status == CoreStatus.StaleExchange)
        continue;
    if (status != CoreStatus.Ok)
        throw new InvalidOperationException($"Response refused: {status}");

    service.BeginDrain();
    var drain = service.WaitDrained(timeoutMilliseconds: 5000);
    service.Stop();
    if (drain.State != DrainState.Complete)
        throw new InvalidOperationException($"Drain ended: {drain.State} ({drain.Status})");

    return;
}

throw new TimeoutException("No completed GET /hello request within 30 seconds.");
```

Run `dotnet run`. When the server prints the listening address, run the
following from a second terminal.

```sh
curl --include --max-time 10 http://127.0.0.1:8080/hello
```

Expect `HTTP/1.1 200` and the body `hello`. Use `curl.exe` in Windows
PowerShell.

`Take` returns a request event with an exchange, which identifies the
request when you call `Respond`. Use one event reader per service. The
`using` declaration disposes the service even if startup or a response fails.

## Choose an HTTP version

| Version | When to use it | Transport |
| --- | --- | --- |
| [HTTP/1.1](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http1.md) | A simple first request or a TCP client using HTTP/1.1 | TCP, with optional TLS |
| [HTTP/2](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http2.md) | Concurrent streams on one HTTPS connection | TCP with TLS |
| [HTTP/3](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http3.md) | Concurrent streams over QUIC when clients can reach the UDP port | UDP with TLS |

For one HTTPS protocol, use `HttpListenerOptions.Standalone` with
`HttpProtocol.Http2` or `HttpProtocol.Http3` and supply a `TlsOptions`
value. The complete sample project runs `http1`, `http2`, `http3` and
`multi` modes. [C# protocol samples](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/csharp)

### Serve all 3 protocols on one port

[Generate the local TLS files](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/README.md#prepare-local-tls-files) next to your project. The certificate and
private key configure the listener. The client uses `ca.pem` to verify it.
Replace `Program.cs` with this complete example. The same program can
run a standalone listener by changing its command-line mode.

```csharp
using System.Text;
using CoAkka.Http;

if (args.Length == 0 || args[0] is not ("http1" or "http2" or "http3" or "multi") ||
    (args[0] == "http1" ? args.Length != 1 : args.Length != 2))
{
    Console.Error.WriteLine("Usage: Server http1 | Server <http2|http3|multi> <certificate-directory>");
    return 2;
}

const string address = "127.0.0.1";
const ulong helloBinding = 1;
string mode = args[0];
ushort port = mode == "http1" ? (ushort)8080 : (ushort)8443;
TlsOptions? tls = mode == "http1" ? null : new TlsOptions(
    "sample", 1, Path.Combine(args[1], "server.pem"), Path.Combine(args[1], "server.key"));
var listener = mode switch
{
    "multi" => HttpListenerOptions.MultiProtocolListener(tls!, bindAddress: address, port: port),
    "http2" => HttpListenerOptions.Standalone(HttpProtocol.Http2, bindAddress: address, port: port, tls: tls),
    "http3" => HttpListenerOptions.Standalone(HttpProtocol.Http3, bindAddress: address, port: port, tls: tls),
    _ => HttpListenerOptions.Standalone(HttpProtocol.Http11, bindAddress: address, port: port),
};

using var service = new HttpService(listener, [new HttpRoute(1, helloBinding, "GET", "/hello")]);
service.Start();
Console.WriteLine($"Listening on {(tls is null ? "http" : "https")}://{address}:{service.Port}/hello");

int remaining = mode == "multi" ? 3 : 1;
byte[] body = Encoding.UTF8.GetBytes("hello");
HttpField[] headers = [new("content-type", "text/plain; charset=utf-8")];
while (remaining > 0)
{
    var request = service.Take(250);
    if (request is not { Kind: EventKind.Request, BindingId: helloBinding })
        continue;

    var status = service.Respond(request.Exchange, 200, body, headers);
    if (status == CoreStatus.StaleExchange)
        continue;
    if (status != CoreStatus.Ok)
        throw new InvalidOperationException($"Response refused: {status}");

    remaining--;
}

service.BeginDrain();
var drain = service.WaitDrained(5000);
service.Stop();
Console.WriteLine($"Drain: {drain.State} ({drain.Status})");
return drain.State == DrainState.Complete ? 0 : 1;
```

From the project directory, run `dotnet run -- multi ./http-test-tls`.
[Verify all 3 versions with curl](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md) from another terminal. The application serves `hello` at
`https://127.0.0.1:8443/hello`. The three requests should report
`200 1.1`, `200 2` and `200 3`. After the third response, the example begins
drain and reports whether it completed.

Run one listener mode at a time. For remote clients, allow both TCP and
UDP on port 8443. Use a supported standalone listener for streaming, SSE,
WebSockets, mTLS or TLS rotation.

## Build on the example

| Task | C# API |
| --- | --- |
| Read path and query parameters, headers and a buffered body | `RequestEvent.PathParameters`, `QueryParameters`, `Headers` and `Body` |
| Receive a streaming upload | Set `HttpRoute.BodyMode` to `RequestBodyMode.Streaming` and handle the DATA, trailers and end events from `Take` |
| Stream a response | `StartResponseStream`, `WriteResponseStream` and `FinishResponseStream` |
| Push server events | Start with `StartServerSentEvents` |
| Serve a WebSocket | Enable `HttpRoute.WebSocket` and read connection events with `TakeWebSocket` |
| Serve files | Configure `StaticMount` for assets or use `RespondFile` with a `FileAuthority` for a file chosen by a handler |
| Read counters | Configure `MonitorOptions` and call `ReadMonitor` |
| Replace a handler | Use `BufferedHandler.SwapHandler` for application dispatch or `RebindHandler` for a supported standalone route |
| Call an upstream service | Configure `OutboundOptions`; submit with `SubmitOutbound`, read the terminal result with `TakeOutbound`, and request cancellation with `CancelOutbound` |

Use the standalone profile for streaming uploads, response streaming,
SSE, WebSockets and file responses. Uploads carry data from the client
into the application. File responses send server-side files back to a
client. [Feature and profile guide](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime/reference/FEATURES.md#what-you-can-use-in-231)

## Diagnose a failed request

- On startup, inspect `CoreException.Status` and `Startup`. Verify the port,
  certificate files and runtime architecture when the service cannot start.
- If `Respond` returns `StaleExchange`, the exchange is already retired.
  Drop its reply. Then keep reading events for other requests.
- If a streamed write is paused by capacity, wait for `ResponseWritable`
  for that exchange. Stop producing when you receive cancellation or a
  terminal event.
- During shutdown keep serving in-flight exchanges while draining. Inspect
  `WaitDrained` before calling `Stop`. An `Expired` drain reports that the
  deadline elapsed with work outstanding.


## Documentation, feedback and support

- [CoAkka HTTP Runtime documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime): concepts, configuration, failure modes and feature recipes.
- [Runnable C# examples](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/csharp): standalone and MultiProtocol applications.
- [Release packages and checksums](https://github.com/phuong-tran/coakka-publish/tree/main/coakka-http-runtime): published artifacts and compatibility information.
- [Contact and support](https://github.com/phuong-tran/coakka-samples/blob/main/docs/common/contact-and-support.md): questions and support requests.

## Support CoAkka

Support development through [GitHub Sponsors](https://github.com/sponsors/phuong-tran)
or [Ko-fi](https://ko-fi.com/phuongnamtran). Both links support the CoAkka ecosystem,
including CoAkka HTTP Runtime and its language connectors.
