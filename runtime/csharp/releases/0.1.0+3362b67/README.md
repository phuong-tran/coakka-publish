# C# Runtime Connector

C#/.NET connector lane for the CoAkka runtime v2.

This lane is local-first and macOS/Linux-first for now. It exists to validate
the .NET API shape before promising the Windows packaging story that .NET users
will reasonably expect later.

Current package shape:

- `RuntimeHost.Start(...)` as the preferred single-process lifecycle entrypoint
- `ConnectorOrchestrator.Start(...)` as a compatibility name over the same
  runtime host
- runtime route snapshots from `ConnectorStartSpec.Routes`
- local JSON request/reply through `AskJsonAsync(...)`
- local handler registration through `RegisterHandler(...)`
- `RuntimeLibraryResolver` for explicit path, `$COAKKA_RUNTIME_LIB`, packaged
  native, and repo-local `lib/` lookup
- direct P/Invoke loading through `NativeLibrary`
- runtime info, config, health, and stats snapshots
- one active runtime host per process
- native package version `0.1.0+3362b67` is preferred from
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

var identity = new PayloadIdentity(
    "samples.customer.create.request.v1",
    1,
    PayloadFormat.Json);

var spec = new ConnectorStartSpec(
    SystemName: "customer-local",
    NodeId: "customer-local-node",
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
    ],
    SeparateDeliveredRequestLane: true);

using var runtime = RuntimeHost.Start(spec);
runtime.RegisterHandler(
    "customers.create",
    request => RuntimeHost.MakeJsonReplyFromRequestIdentity(
        request,
        "customers.create",
        customerStore.Create(request.PayloadUtf8())));

var response = await runtime.AskJsonAsync(
    source: "customer-local",
    target: "customers.create",
    payload: command,
    payloadIdentity: identity,
    deliveryHint: DeliveryHint.RequireLocal);
```

`ConnectorOrchestrator` remains available for code that wants the older
connector vocabulary. New local-first ASP.NET Core, CRUD, and worker examples
should prefer `RuntimeHost` so the file reads as one embedded runtime owner,
not a remote connector setup.

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
csharp/build/nupkg/CoAkka.Runtime.0.1.1.nupkg
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
3. staged native under `v2/staging/native/0.1.0+3362b67/<platform>/`
4. packaged native under `native/<platform>/`
5. repo-local `lib/`

Current supported platform IDs:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`

Windows is intentionally not claimed yet. It needs a separate native DLL
packaging and smoke pass before this lane should be presented as a .NET
production surface.
