# C# Runtime Transport Configuration

The public CoAkka C ABI defines lifecycle, capability, validation, atomicity,
and edition behavior. The C# connector copies every public snapshot/result and
does not retain native pointers or expose secret paths/material.

## Startup

Set `ConnectionStrategy` and `Security` on `ConnectorStartSpec`. The connector
creates the runtime, applies selected transport policy in `CREATED`, exports
host handles, applies routes, and starts. Omit either value to preserve runtime
defaults. A rejected startup apply destroys the native handle and throws
`RuntimeTcpConnectionApplyException` or `RuntimeTcpSecurityApplyException` with
the copied structured result.

```csharp
var capabilities = RuntimeHost.ReadRuntimeCapabilities(runtimeLibrary);
var mode = capabilities.Supports(RuntimeCapability.TcpBoundedPool)
    ? RuntimeTcpConnectionMode.BoundedPool
    : RuntimeTcpConnectionMode.PerExchange;

using var runtime = RuntimeHost.Start(new ConnectorStartSpec(
    SystemName: "orders",
    NodeId: "orders-1",
    Routes: [RuntimeHost.LocalRoute("orders.create")],
    ConnectionStrategy: new RuntimeTcpConnectionStrategySpec(mode),
    Security: new RuntimeTcpSecuritySpec(RuntimeTcpSecurityMode.Plaintext)),
    runtimeLibrary);
```

## Public Functions

| Function | Purpose, parameters, and default | Ownership, thread safety, blocking, lifecycle, atomicity, errors, edition |
| --- | --- | --- |
| `RuntimeHost.ReadRuntimeCapabilities(path)` | Resolves one library and copies compiled/entitled/effective bits. Null uses resolver order. | Synchronous and process-safe. First module identity stays loaded; a different later path is rejected. All editions. |
| `capabilities.Supports(bits)` | Requires every requested effective bit. Empty set is true. | Pure copied-value operation; never infers support from package/profile names. |
| `RuntimeHost.Start(spec, path)` | Owns one active process host. Optional transport specs preserve defaults when null. | Applies before start. Credential file loading may block. Rejection returns typed structured exception and releases runtime ownership. |
| `runtime.RuntimeCapabilities()` | Copies capability truth through the host binding. | Serialized with transport apply/dispose; open host only; all editions. |
| `runtime.TcpConnectionConfig()` | Copies effective mode, tuning, and explicit/default provenance. | Serialized, synchronous, open host only; availability follows capability bits. |
| `runtime.TcpSecurityInfo()` | Copies active non-secret mode, generation, ID, protocol/verification, validity, fingerprint. | No paths, PEM, keys, tokens, or raw diagnostics. Serialized; TLS fields capability-gated. |
| `runtime.StartupTcpConnectionResult()` | Returns explicit startup connection result. | Null when omitted; immutable copied value remains safe after dispose. |
| `runtime.StartupTcpSecurityResult()` | Returns explicit startup security result. | Null when omitted; immutable non-secret copied value remains safe after dispose. |
| `runtime.ApplyTcpConnectionStrategy(spec)` | Attempts apply; nullable tuning means absent. | Serialized and atomic. Connection strategy is startup-only, so started hosts normally return `BadState` while preserving active config. |
| `runtime.ApplyTcpSecurity(spec)` | Applies startup-shaped security or same-mode newer-generation reload. | Strings are borrowed only for the synchronous call. File I/O may block. Failed/stale reload preserves active generation. TLS/mTLS/reload are capability-gated. |
| `result.Applied` | Tests native status `Ok`; `Changed` is independent. | Pure copied-value property; a successful no-op may be applied without changing state. |
| `RuntimeTransportAbi.Sizes()` | Returns managed sizes for eleven public blocks. | Diagnostic/layout evidence only; it does not prove execution on another OS/CPU. |

`ConnectorOrchestrator` forwards the host getters, startup results, and apply
methods without changing semantics.

## Specs And Defaults

`RuntimeTcpConnectionStrategySpec` defaults to `PerExchange`. Null tuning
fields omit presence bits; non-null zero is explicit and core may reject it.
`BoundedPool`, `PersistentSingleFlight`, and `Multiplexing` must be selected
only after capability discovery. Tuning is independently gated.

`RuntimeTcpSecuritySpec` defaults to plaintext, graceful reload, generation
zero, and empty credential fields. TLS/mTLS requires file-backed CA,
certificate, private key, nonempty credential ID, and positive generation.
Plaintext credential/reload fields are forwarded with presence bits so core
returns a structured rejection instead of silently discarding them.

## TLS Ownership And Reload

```csharp
var result = runtime.ApplyTcpSecurity(new RuntimeTcpSecuritySpec(
    Mode: RuntimeTcpSecurityMode.Tls,
    CredentialGeneration: 2,
    CredentialId: "orders-2026-08",
    CaCertificateFile: "/run/secrets/coakka/ca.pem",
    IdentityCertificateFile: "/run/secrets/coakka/tls.pem",
    PrivateKeyFile: "/run/secrets/coakka/tls.key"));
```

The caller owns strings and secret-file lifecycle. Native code loads and
validates them synchronously, then atomically publishes an immutable context.
A rejected mismatch or stale generation leaves the previous active context.
See the canonical [TLS/mTLS guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
for direct runtime TLS/mTLS without a service-mesh data plane, Kubernetes,
controlled networks, LAN/edge, RPi, BeagleBone, bare metal, industrial Android,
and rotation semantics.

## Platforms And Troubleshooting

The NuGet package digest-verifies `osx-arm64`, `linux-arm64`, and `win-x64`.
Payload presence is not execution evidence; exact connector and consumer
evidence is reported per platform. Use [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for RID/loader, OS/CPU, dependencies, certificates, Gatekeeper, Authenticode,
digests, and signing status. Publisher signing is currently absent.
Contact `gabrielgun1983@gmail.com` or use the public issue tracker.
