# Consuming The C# Runtime Connector

This is the C#/.NET runtime connector for CoAkka runtime v2.

macOS and Windows are supported development and validation hosts for this
connector shape. The current public release directories still bundle native
runtime libraries for macOS/Linux first, and Linux remains the default server
deployment path.

## Install From A Local Release Directory

Point a local NuGet source at the release directory that contains
`CoAkka.Runtime.1.3.2.nupkg`, then add the package:

```sh
dotnet nuget add source /path/to/release-dir --name coakka-local
dotnet add package CoAkka.Runtime --version 1.3.2 --source /path/to/release-dir
```

The package includes native runtime libraries for:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

The runtime library resolution order is:

1. explicit `runtimeLibPath`
2. `$COAKKA_RUNTIME_LIB`
3. staged or packaged `native/<platform>/`
4. repo-local `lib/`

## Before

Without the local runtime host shape, .NET code tends to create a backend
HTTP/gRPC boundary even when the work is still in the same process or app
owner:

```csharp
app.MapPost("/api/customers", async (
    CreateCustomerRequest request,
    HttpClient backendClient) =>
{
    var response = await backendClient.PostAsJsonAsync(
        "http://customer-store/backend/customers",
        request);
    response.EnsureSuccessStatusCode();
    return Results.Json(await response.Content.ReadFromJsonAsync<CustomerDto>());
});
```

That shape adds URL config, HTTP serialization, timeout/error mapping, retry
policy, and test setup before there is a real process or network boundary.

## After

With CoAkka, ASP.NET Core can keep HTTP at the real edge while the process owns
one embedded runtime host and registers application work as a local capability:

```csharp
using CoAkka.Runtime;

const string target = "customers.greet";

using var runtime = RuntimeHost.StartLocal("customer-api", target);
runtime.RegisterTextHandler(target, name => $"Hello {name}");

var response = await runtime.AskTextAsync(
    source: "customer-api",
    target: target,
    payload: "Ada",
    payloadIdentity: PayloadIdentity.Text("customers.greet.request.v1"),
    deliveryHint: DeliveryHint.RequireLocal);
```

`RuntimeHost.StartLocal(...)` and `RuntimeHost.LocalRoute(...)` now auto-select
one free IPv4 loopback port when `diagnosticPort` is omitted or set to `0`, so
the first-run local path does not assume an ambient privileged port.

Use the explicit route/envelope layer when the handler needs typed JSON payloads
or custom route metadata:

```csharp
using CoAkka.Runtime;

var identity = PayloadIdentity.Json("samples.customer.create.request.v1");
var spec = new ConnectorStartSpec(
    SystemName: "customer-api",
    NodeId: "customer-api-node",
    Routes: [RuntimeHost.LocalRoute("customers.create", diagnosticPort: 19141)]);

using var runtime = RuntimeHost.Start(spec);

runtime.RegisterHandler(
    "customers.create",
    request => RuntimeHost.MakeJsonReplyFromRequestIdentity(
        request,
        "customers.create",
        customerStore.Create(request.PayloadUtf8())));

var response = await runtime.AskJsonAsync(
    source: "customer-api",
    target: "customers.create",
    payload: command,
    payloadIdentity: identity,
    deliveryHint: DeliveryHint.RequireLocal);
```

ASP.NET Core DI and `[CoAkkaHandler]`-style capability scanning are intentionally
next. This package now proves the connector-level runtime host, route snapshot,
request/reply, handler, matched-deadletter, and native packaging baseline.
