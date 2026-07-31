# CoAkka.Runtime

.NET connector package for CoAkka runtime v2.

This package targets `net10.0` and bundles native runtime generation
`1.3.4+dc6ec284` for macOS ARM64, Linux x64/ARM64, and Windows x64/ARM64.

## New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. It helps an app route work by target name, handle request/reply,
deadletters, bounded queues, diagnostics, and native-backed logging without
turning every internal boundary into another hand-written HTTP endpoint.

Use these public repositories to orient first:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-samples` | Runnable examples and code you can inspect first. | https://github.com/phuong-tran/coakka-samples |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |

Run the matching sample:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh runtime csharp basic
```

C# runtime sample docs:
https://github.com/phuong-tran/coakka-samples/tree/main/runtime/csharp

Try the package without cloning any CoAkka repo after the NuGet release is
published:

```sh
mkdir coakka-runtime-csharp-first-run
cd coakka-runtime-csharp-first-run
dotnet new console --framework net10.0
dotnet add package CoAkka.Runtime --version 1.3.5
```

## Quick Start

```csharp
using CoAkka.Runtime;

const string target = "samples.customer.store.create";
var store = new Dictionary<string, object>();

using var runtime = RuntimeHost.StartLocal("customer-app", target);

runtime.RegisterHandler(
    target,
    request =>
    {
        var customer = new
        {
            id = "cust-001",
            name = request.PayloadUtf8(),
            createdBy = request.Source,
        };
        store[customer.id] = customer;

        return RuntimeHost.MakeJsonReplyFromRequestIdentity(
            request,
            target,
            new { status = "created", customer, storedCount = store.Count });
    });

var response = await runtime.AskJsonAsync(
    source: "customer-api",
    target: target,
    payload: new { id = "cust-001", name = "Ada Lovelace" },
    payloadIdentity: PayloadIdentity.Json("samples.customer.create.request.v1"),
    timeoutMs: 2_000,
    operation: "create_customer",
    deliveryHint: DeliveryHint.RouterDefault);

Console.WriteLine(response.PayloadUtf8());
```

One process owns one active runtime host. Start the host, register handlers for
targets this process owns, send typed requests to target names, then dispose
the host during application shutdown.

## Before / After

CoAkka is easiest to understand from a fake backend HTTP handoff. The browser
or public API edge can be real HTTP and should stay HTTP. The fake part is the
second private endpoint a team adds only so app-owned store work has something
URL-shaped to call:

```csharp
app.MapPost("/backend/customers", async (
    CreateCustomerRequest request,
    CustomerStore store) =>
{
    var customer = await store.Create(request);
    return Results.Json(customer);
});
```

The caller now owns URL config, HTTP parsing, status mapping, timeout mapping,
retry policy, logs, and test fixtures for a boundary that is not really a
public API:

```csharp
app.MapPost("/api/customers", async (
    CreateCustomerRequest request,
    HttpClient backendClient) =>
{
    var reply = await backendClient.PostAsJsonAsync(
        "http://customer-store/backend/customers",
        request);
    reply.EnsureSuccessStatusCode();
    return Results.Json(await reply.Content.ReadFromJsonAsync<CustomerDto>());
});
```

With CoAkka, the public edge can stay HTTP, but the fake backend URL becomes a
runtime target owned by the app:

```csharp
app.MapPost("/api/customers", async (
    CreateCustomerRequest request,
    RuntimeHost runtime) =>
{
    var response = await runtime.AskJsonAsync(
        source: "customer-api",
        target: "samples.customer.store.create",
        payload: request,
        payloadIdentity: PayloadIdentity.Json("samples.customer.create.request.v1"),
        timeoutMs: 5_000,
        operation: "create_customer",
        deliveryHint: DeliveryHint.RouterDefault);

    return Results.Json(response.PayloadUtf8());
});
```

The change is not "replace HTTP." HTTP still belongs at real browser/API or
legacy edges. CoAkka removes backend HTTP that exists only to call capabilities
owned by the same app or team by URL.

## Package Shape

- `RuntimeHost.StartLocal(...)` for one same-process target
- `RuntimeHost.LocalRoute(...)` with auto-selected loopback diagnostic port
  when `diagnosticPort` is omitted or `0`
- `RegisterTextHandler(...)`, `AskTextAsync(...)`, and
  `PayloadIdentity.Text(...)` for the smallest practice path
- `RegisterHandler(...)`, `AskJsonAsync(...)`, and
  `PayloadIdentity.Json(...)` for typed JSON request/reply paths
- `RuntimeHost.Start(...)` for explicit route snapshots and advanced start
  options
- `ConnectorOrchestrator.Start(...)` as a compatibility name over the same
  runtime host
- matched deadletters surfaced as `DeadletterException`
- runtime info, config, health, stats, and request/reply counters
- native library resolution through explicit `runtimeLibPath`,
  `$COAKKA_RUNTIME_LIB`, NuGet RID native assets, repo staging, or local `lib/`

Supported NuGet runtime identifiers:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

## Development

Build:

```sh
dotnet build csharp/CoAkka.Runtime/CoAkka.Runtime.csproj
```

Pack:

```sh
bash csharp/scripts/build-package.sh
```

The package is written to:

```text
csharp/build/nupkg/CoAkka.Runtime.1.3.5.nupkg
```

Readiness gate:

```sh
bash csharp/scripts/check-nuget-readiness.sh
```

Packaged consumer smoke:

```sh
bash csharp/scripts/smoke-packaged-package.sh
```

NuGet release steps live in [RELEASE.md](RELEASE.md). The guarded publish
script requires `NUGET_API_KEY` and
`COAKKA_NUGET_PUBLISH_CONFIRM=CoAkka.Runtime/1.3.5` before it pushes anything
to nuget.org.
