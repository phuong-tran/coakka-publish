# Consuming CoAkka.Runtime

`CoAkka.Runtime` is the .NET connector for the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem.

Connector version `1.4.5` uses exact native generation
`1.4.0+2cee86bf`.

## Local Package Verification

Build the package from the connector source checkout:

```sh
bash csharp/scripts/build-package.sh
```

Point a local NuGet source at the generated package directory:

```sh
dotnet nuget add source /path/to/coakkaJVMConnector/csharp/build/nupkg --name coakka-local
dotnet add package CoAkka.Runtime --version 1.4.5 --source coakka-local
```

For a registry package, install a compatible version:

```sh
dotnet add package CoAkka.Runtime --version <compatible-version>
```

The package includes native runtime libraries for:

- `osx-arm64`
- `linux-arm64`
- `win-x64`

The package verifier requires all three exact release-metadata digests. Current
execution evidence covers macOS ARM64; this package receipt makes no C#
connector/consumer execution claim for Linux ARM64 or Windows x86-64. Use
[Transport Configuration](TRANSPORT_CONFIGURATION.md) and
[common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for loader, RID, dependency, certificate, Gatekeeper, Authenticode, digest, and
currently absent publisher-signing failures.

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
4. source-checkout staging configured by the package build environment
5. source-checkout `csharp/CoAkka.Runtime/native/<platform>`
6. local `lib/`

Public samples:
https://github.com/phuong-tran/coakka-samples/tree/main/runtime/csharp
