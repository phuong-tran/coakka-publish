# CoAkka.Runtime

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

**This is the .NET/C# connector in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem.** CoAkka is not a .NET-only runtime:
this package adapts .NET applications to the same native core, public C ABI,
target, request/reply, bounded-admission, and deadletter contract used by the
JVM, Node.js, Python, Go, Rust, Swift, and other connector lanes.

This package targets `net10.0`. Kubernetes is supported but not required. Use
the public [Ecosystem Overview](https://github.com/phuong-tran/coakka-publish/blob/main/docs/ecosystem-overview.md)
and [Compatibility Matrix](https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md)
to select an exact NuGet package and native RID payload.
Start with the [CoAkka Documentation](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
for concepts, integration paths, operations, and runnable samples.

Common guidance:

- [Connection strategies](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md)
- [TLS and mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
- [Contact and support](https://github.com/phuong-tran/coakka-publish/blob/main/docs/contact-and-support.md): `gabrielgun1983@gmail.com`
- [C# transport API](TRANSPORT_CONFIGURATION.md)

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

Use the current compatible version shown in `coakka-publish` for registry
installation.

```sh
mkdir coakka-runtime-csharp-first-run
cd coakka-runtime-csharp-first-run
dotnet new console --framework net10.0
dotnet add package CoAkka.Runtime --version <compatible-version>
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
- capability discovery before optional transport selection
- startup-configured connection/security policy with copied structured results
- same-mode newer-generation TLS/mTLS credential reload; rejection preserves
  active state
- native library resolution through explicit `runtimeLibPath`,
  `$COAKKA_RUNTIME_LIB`, NuGet RID native assets, repo staging, or local `lib/`

Native runtime-v2 RID payloads:

- `osx-arm64`
- `linux-arm64`
- `win-x64`

All three are distribution targets and their release metadata digests are
verified before pack. Cross-publish selects the exact Linux ARM64 ELF and
Windows x86-64 PE assets. Current C# execution evidence is macOS ARM64; this
package receipt makes no C# connector/consumer execution claim for Linux or
Windows. Payload selection does not claim execution on a target.

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
csharp/build/nupkg/CoAkka.Runtime.1.4.5.nupkg
```

Readiness gate:

```sh
bash csharp/scripts/check-nuget-readiness.sh
```

Packaged consumer smoke:

```sh
bash csharp/scripts/smoke-packaged-package.sh
```

NuGet packaging and verification steps live in [RELEASE.md](RELEASE.md).
