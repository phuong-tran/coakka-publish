# Consuming CoAkka.Runtime

`CoAkka.Runtime` is the .NET connector for the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem.

The current public release is connector version `2.5.3` over exact native
generation `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`.

The package provides one `net8.0` managed asset and supports applications
targeting `net8.0`, `net9.0`, or `net10.0`. The release gate builds and executes
all three consumers against the same package file.

## Local Package Verification

Build the package from the connector source checkout:

```sh
bash csharp/scripts/build-package.sh
```

Point a local NuGet source at the generated package directory:

```sh
dotnet nuget add source /path/to/coakkaJVMConnector/csharp/build/nupkg --name coakka-local
dotnet add package CoAkka.Runtime --version 2.5.3 --source coakka-local
```

Install the current registry package:

```sh
dotnet add package CoAkka.Runtime --version 2.5.3
```

The package includes native runtime libraries for:

- `linux-arm64`
- `linux-x64`
- `osx-arm64`
- `win-arm64`
- `win-x64`

The package verifier requires all five exact release-metadata digests. Exact
NuGet `CoAkka.Runtime==2.5.3` request/reply and route-miss deadletter execution
passes on macOS ARM64, Linux ARM64/x86-64, and Windows ARM64. Windows x86-64
has package verification but no matching-host execution record. Use
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

`RuntimeHost.StartLocal(...)` and `RuntimeHost.LocalRoute(...)` use embedded mode
and local endpoint port `0` by default. They do not reserve or open a TCP port.

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
