# Consuming CoAkka.Runtime

`CoAkka.Runtime` is the .NET connector package for CoAkka runtime v2.

The current NuGet-ready candidate is `CoAkka.Runtime` version `1.3.5`, bundled
with native runtime generation `1.3.4+dc6ec284`.

## Local Candidate Install

Build the package from the connector source checkout:

```sh
bash csharp/scripts/build-package.sh
```

Point a local NuGet source at the generated package directory:

```sh
dotnet nuget add source /path/to/coakkaJVMConnector/csharp/build/nupkg --name coakka-local
dotnet add package CoAkka.Runtime --version 1.3.5 --source coakka-local
```

After the NuGet release is published, install from the normal registry:

```sh
dotnet add package CoAkka.Runtime --version 1.3.5
```

Release operators should use [RELEASE.md](RELEASE.md) instead of calling
`dotnet nuget push` directly.

The package includes native runtime libraries for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

## Local Runtime Host

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

Console.WriteLine(response);
```

Use the explicit route/envelope layer when the handler needs typed JSON payloads
or custom route metadata:

```csharp
using CoAkka.Runtime;

var identity = PayloadIdentity.Json("samples.customer.create.request.v1");
var spec = new ConnectorStartSpec(
    SystemName: "customer-api",
    NodeId: "customer-api-node",
    Routes: [RuntimeHost.LocalRoute("customers.create")]);

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

`RuntimeHost.StartLocal(...)` and `RuntimeHost.LocalRoute(...)` auto-select one
free IPv4 loopback port when `diagnosticPort` is omitted or set to `0`.

## Native Resolution

The runtime library resolution order is:

1. explicit `runtimeLibPath`
2. `$COAKKA_RUNTIME_LIB`
3. NuGet native assets under `runtimes/<rid>/native`
4. source-checkout staging under `v2/staging/native/1.3.4+dc6ec284/<platform>`
5. source-checkout `csharp/CoAkka.Runtime/native/<platform>`
6. local `lib/`

Public samples:
https://github.com/phuong-tran/coakka-samples/tree/main/runtime/csharp
