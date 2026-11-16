# CoAkka HTTP Runtime for C#

![CoAkka](https://raw.githubusercontent.com/phuong-tran/coakka-samples/a539dfcd7dc8d93c5b1c61b1324899af13d47ca6/docs/common/assets/brand/coakka-logo.png)

CoAkka HTTP Runtime is an embeddable HTTP server for .NET applications.
Use C# handlers to serve APIs, accept uploads, stream responses, push
updates with Server-Sent Events (SSE) or WebSockets, and deliver files.
CoAkka HTTP Runtime handles HTTP connections, request limits and shutdown.
Your application defines the routes and the responses.

The CoAkka ecosystem provides polyglot tools for distributed applications,
HTTP services and logging. You can adopt CoAkka HTTP Runtime independently
and run the same HTTP protocols across different application languages.

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [HTTP documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran)

## Install 2.3.1

Use .NET 8 or a compatible newer .NET version. The NuGet package targets
`net8.0` and includes the native libraries for the application architecture.

| Platform | Architecture | Notes |
| --- | --- | --- |
| macOS | ARM64 | Use .NET 8 or newer |
| Linux | x86-64 and ARM64 | glibc 2.28 or newer |
| Windows | x64 and ARM64 | Use a toolchain for the application architecture |

Create a console project, then add the package from NuGet.

```sh
dotnet new console --name HelloServer --framework net8.0
cd HelloServer
dotnet add package coakka-http-runtime-csharp --version 2.3.1
```

Install `coakka-http-runtime-csharp` and import `CoAkka.Http`.

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

Generate the [local TLS files](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/README.md#prepare-local-tls-files).
Use the certificate chain and private key on the server, and `ca.pem`
for trust in the client. The credential ID names the certificate
configuration. The generation identifies its revision.

```csharp
var tls = new TlsOptions(
    CredentialId: "sample",
    Generation: 1,
    CertificateChainFile: "http-test-tls/server.pem",
    PrivateKeyFile: "http-test-tls/server.key");
var listener = HttpListenerOptions.MultiProtocolListener(
    tls, bindAddress: "127.0.0.1", port: 8443);
```

For the protocol sample project, use `dotnet run --project csharp --
multi ./http-test-tls` from `protocol-listeners` with the certificates at
that path. The sample stays up for all 3 probes.

The shared listener serves buffered routes over HTTPS on one TCP and UDP
port. Start it once and run the 3 probes against the same URL. Use a curl
build that lists `HTTP2` and `HTTP3` in `curl --version`.

```sh
for protocol in --http1.1 --http2 --http3-only; do
  curl --noproxy '*' "$protocol" --cacert ./http-test-tls/ca.pem \
    --fail --silent --show-error --max-time 10 \
    --write-out '\n%{http_code} %{http_version}\n' https://127.0.0.1:8443/hello || exit
done
```

Each call returns `hello`. The final lines should be `200 1.1`, `200 2`
and `200 3`. If curl reports `200 1.1` for the second call, check the
HTTP/2 negotiation. If HTTP/3 times out, check that UDP 8443 reaches
the server. [Full shared-listener exercise](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md)

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

Connector 2.3.1 uses Core SDK 2.3.0 (C ABI 10).
