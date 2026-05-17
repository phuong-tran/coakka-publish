# Consuming The C# Runtime Connector

This is the experimental C#/.NET runtime connector for CoAkka runtime v2.

The first release is macOS/Linux-first. It does not claim Windows support yet.
That is intentional: the current goal is to make the .NET local runtime shape
obvious before adding Windows DLL packaging.

## Install From A Local Release Directory

Point a local NuGet source at the release directory that contains
`CoAkka.Runtime.0.1.1.nupkg`, then add the package:

```sh
dotnet nuget add source /path/to/release-dir --name coakka-local
dotnet add package CoAkka.Runtime --version 0.1.1 --source /path/to/release-dir
```

The package includes native runtime libraries for:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`

The runtime library resolution order is:

1. explicit `runtimeLibPath`
2. `$COAKKA_RUNTIME_LIB`
3. staged or packaged `native/<platform>/`
4. repo-local `lib/`

## Before

Without the local runtime host shape, .NET code tends to create an internal
HTTP/gRPC boundary even when the work is still in the same process or app
owner:

```csharp
app.MapPost("/api/customers", async (
    CreateCustomerRequest request,
    HttpClient internalClient) =>
{
    var response = await internalClient.PostAsJsonAsync(
        "http://customer-store/internal/customers",
        request);
    response.EnsureSuccessStatusCode();
    return Results.Json(await response.Content.ReadFromJsonAsync<CustomerDto>());
});
```

That shape adds URL config, HTTP serialization, timeout/error mapping, retry
policy, and test setup before there is a real process or network boundary.

## After

With CoAkka, ASP.NET Core can keep HTTP at the real edge while the process owns
one embedded runtime host and registers internal work as a local capability:

```csharp
using CoAkka.Runtime;

var identity = new PayloadIdentity(
    "samples.customer.create.request.v1",
    1,
    PayloadFormat.Json);

var spec = new ConnectorStartSpec(
    SystemName: "customer-api",
    NodeId: "customer-api-node",
    Routes:
    [
        new RuntimeRouteSpec(
            Target: "customers.create",
            Endpoints:
            [
                new RuntimeEndpointSpec(
                    Host: "127.0.0.1",
                    Port: 19141,
                    Flags: RuntimeEndpointFlags.Local),
            ])
    ]);

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
