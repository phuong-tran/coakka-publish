# C# Runtime Connector

C#/.NET connector lane for the CoAkka runtime v2.

This lane is local-first. macOS and Windows are both valid development and
validation hosts for the .NET API shape, while Linux remains the default
deployment path and the main packaged runtime target for server rollout.

Current package shape:

- `RuntimeHost.StartLocal(...)` for one same-process target
- `RuntimeHost.LocalRoute(...)` so first-run code does not spell host/port placeholders,
  and omitting the port auto-selects one free IPv4 loopback port
- `RegisterTextHandler(...)`, `AskTextAsync(...)`, and `PayloadIdentity.Text(...)`
  for the smallest practice path
- `RuntimeHost.Start(...)` as the preferred single-process lifecycle entrypoint
- `ConnectorOrchestrator.Start(...)` as a compatibility name over the same
  runtime host
- runtime route snapshots from `ConnectorStartSpec.Routes`
- local JSON request/reply through `AskJsonAsync(...)`
- local handler registration through `RegisterHandler(...)`
- `RuntimeLibraryResolver` for explicit path, `$COAKKA_RUNTIME_LIB`, packaged
  native, staged Windows/macOS/Linux natives, and repo-local `lib/` lookup
- direct P/Invoke loading through `NativeLibrary`
- runtime info, config, health, and stats snapshots
- delivered-request lane enabled by default for request/reply hosts, with an
  advanced override for measured one-way-only hosts
- one active runtime host per process
- native package version `0.2.0+b8ecfae` is preferred from
  `v2/staging/native/` before falling back to repo-local `lib/`

ASP.NET Core DI, `[CoAkkaHandler]`-style capability scanning, and public
polyglot samples come after this connector-level request/reply lane proves out.

## Before / After

Before, a local .NET consumer would start from a connector-internal
orchestration name:

```csharp
using CoAkka.Runtime;

var spec = new ConnectorStartSpec(
    SystemName: "customer-local",
    NodeId: "customer-local-node");

using var connector = ConnectorOrchestrator.Start(spec);
var info = connector.RuntimeInfo();
Console.WriteLine(info.SouthboundBackend);
```

After, the same single-process runtime is named as the application-owned host,
and internal work can be registered as a local capability:

```csharp
using CoAkka.Runtime;

const string target = "customers.greet";

using var runtime = RuntimeHost.StartLocal("customer-local", target);
runtime.RegisterTextHandler(target, name => $"hello {name}");

var response = await runtime.AskTextAsync(
    source: "customer-local",
    target: target,
    payload: "Ada",
    payloadIdentity: PayloadIdentity.Text("customers.greet.request.v1"),
    deliveryHint: DeliveryHint.RequireLocal);
```

`ConnectorOrchestrator` remains available for code that wants the older
connector vocabulary. New local-first ASP.NET Core, CRUD, and worker examples
should prefer `RuntimeHost` so the file reads as one embedded runtime owner,
not a remote connector setup.

For explicit JSON envelopes, keep the advanced route/envelope surface:

```csharp
var identity = PayloadIdentity.Json("samples.customer.create.request.v1");
var spec = new ConnectorStartSpec(
    SystemName: "customer-local",
    NodeId: "customer-local-node",
    Routes: [RuntimeHost.LocalRoute("customers.create", diagnosticPort: 19141)]);

using var runtime = RuntimeHost.Start(spec);
runtime.RegisterHandler(
    "customers.create",
    request => RuntimeHost.MakeJsonReplyFromRequestIdentity(
        request,
        "customers.create",
        customerStore.Create(request.PayloadUtf8())));
```

`SeparateDeliveredRequestLane` defaults to `true`. Most request/reply services
should leave it alone so inbound handler work stays separate from
reply/deadletter matching. Set it to `false` only for advanced, measured,
mostly one-way hosts.

This is intentionally connector-level first. A later ASP.NET Core adapter can
turn the same handler shape into DI and `[CoAkkaHandler]` scanning without
changing the runtime wire contract.

## Build

```sh
dotnet build csharp/CoAkka.Runtime/CoAkka.Runtime.csproj
```

## Package

```sh
bash csharp/scripts/build-package.sh
```

The NuGet package is written to:

```text
csharp/build/nupkg/CoAkka.Runtime.0.2.0.nupkg
```

## Smoke

From the repo root:

```sh
dotnet run --project csharp/CoAkka.Runtime.Smoke/CoAkka.Runtime.Smoke.csproj
```

Packaged consumer smoke:

```sh
bash csharp/scripts/smoke-packaged-package.sh
```

The smoke resolves the native runtime from:

1. explicit `runtimeLibPath`
2. `$COAKKA_RUNTIME_LIB`
3. staged native under `v2/staging/native/0.2.0+b8ecfae/<platform>/`
4. packaged native under `native/<platform>/`
5. repo-local `lib/`

Current supported platform IDs:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`
