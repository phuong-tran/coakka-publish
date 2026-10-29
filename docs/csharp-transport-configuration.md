# C# Runtime Transport Configuration

The public CoAkka C ABI defines lifecycle, capability, validation, atomicity,
and edition behavior. The C# connector copies every public snapshot/result and
does not retain native pointers or expose secret paths/material.

## Startup

Unreleased native-client source candidate: connection intent is submitted at
Core construction, not reapplied after Core freezes startup selection. Omitted
connection/execution groups select Core AUTO/MULTIPLEXING. Construction failure
preserves Core's status but has no active instance/apply-result record;
`StartupTcpConnectionResult()` therefore returns null, even for explicit intent.
Read `StartupConfig().Connection` or `TcpConnectionConfig()` for accepted facts.
Security still uses Core's structured pre-start apply result. Inputs are not
trimmed/repaired by the host; only host representation/NUL/UTF-8 checks precede
Core validation. Explicit inapplicable network fields reach Core and reject,
instead of being silently omitted. See the candidate lifecycle/shrink bounds in
the C# README. Monitor intent is nullable: omission selects Core defaults while
explicit false remains explicit off in Core provenance. A connection-spec value
requires an explicit mode; omit the whole spec to request Core defaults rather
than copying a default-mode table into the connector.
Core constructs connection/execution/I/O/monitor settings; security is a
separate structured pre-start apply. Startup failure retires partial native
ownership. A construction refusal preserves its status without fabricating an
apply result; a security apply refusal retains its actual structured result.
Normal callers omit advanced groups; they do not choose a fallback mode from
capability bits or mirror Core's default table.

```csharp
using var runtime = RuntimeHost.Start(new ConnectorStartSpec(
    SystemName: "orders",
    NodeId: "orders-1",
    Routes: [RuntimeHost.LocalRoute("orders.create")]),
    runtimeLibrary);
```

## Public Functions

| Function | Purpose, parameters, and default | Ownership, thread safety, blocking, lifecycle, atomicity, errors, edition |
| --- | --- | --- |
| `RuntimeHost.ReadRuntimeCapabilities(path)` | Resolves one library and copies compiled/entitled/effective bits. Null uses resolver order. | Synchronous and process-safe. First module identity stays loaded; a different later path is rejected. All editions. |
| `capabilities.Supports(bits)` | Requires every requested effective bit. Empty set is true. | Pure copied-value operation; never infers support from package/profile names. |
| `RuntimeHost.Start(spec, path)` | Creates independent hosts over one process-retained module. Omitted groups ask Core for defaults. | Native construction precedes security apply/routes/start. Credential loading may block; failure retires partial ownership. |
| `runtime.RuntimeCapabilities()` | Copies capability truth through the host binding. | Serialized with transport apply/dispose; open host only; all editions. |
| `runtime.TcpConnectionConfig()` | Copies effective mode, tuning, and explicit/default provenance. | Serialized, synchronous, open host only; availability follows capability bits. |
| `runtime.TcpSecurityInfo()` | Copies active non-secret mode, generation, ID, protocol/verification, validity, fingerprint. | No paths, PEM, keys, tokens, or raw diagnostics. Serialized; TLS fields capability-gated. |
| `runtime.StartupTcpConnectionResult()` | Always null for native construction: no connection apply occurred. | Read Core startup/connection getters for accepted facts, including explicit intent; never synthesize an apply result. |
| `runtime.StartupTcpSecurityResult()` | Returns explicit startup security result. | Null when omitted; immutable non-secret copied value remains safe after dispose. |
| `runtime.ApplyTcpConnectionStrategy(spec)` | Attempts apply; nullable tuning means absent. | Serialized and atomic. Connection strategy is startup-only, so started hosts normally return `BadState` while preserving active config. |
| `runtime.ApplyTcpSecurity(spec)` | Applies startup-shaped security or same-mode newer-generation reload. | Strings are borrowed only for the synchronous call. File I/O may block. Failed/stale reload preserves active generation. TLS/mTLS/reload are capability-gated. |
| `result.Applied` | Tests native status `Ok`; `Changed` is independent. | Pure copied-value property; a successful no-op may be applied without changing state. |
| `RuntimeTransportAbi.Sizes()` | Returns managed sizes for eleven public blocks. | Diagnostic/layout evidence only; it does not prove execution on another OS/CPU. |

`ConnectorOrchestrator` forwards the host getters, startup results, and apply
methods without changing semantics.

## Specs And Defaults

A supplied `RuntimeTcpConnectionStrategySpec` requires an explicit mode. Omit
the whole group for Core's default MULTIPLEXING; omitted execution asks Core
for AUTO. Null tuning fields omit presence bits; non-null zero is explicit and
Core may reject it. Core validates explicit mode/tuning capability and lifecycle;
the connector never silently substitutes a fallback mode.

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
for Kubernetes ingress/service mesh, controlled networks, LAN/edge, RPi,
BeagleBone, bare metal, industrial Android, and rotation semantics.

## Platforms And Troubleshooting

The private `3.0.0` candidate digest-verifies its five native payloads:
`linux-arm64`, `linux-x64`, `osx-arm64`, `win-arm64` and `win-x64`, over
Core `1522b4a9` / ABI1. The exact candidate qualification record identifies
which hosts have actually executed this package. Core private dependencies
are absorbed; consumers do not install them separately.
Payload presence is not execution evidence; exact connector and consumer
evidence is reported per platform. Use [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for RID/loader, OS/CPU, dependencies, certificates, Gatekeeper, Authenticode,
digests, and signing status. Publisher signing is currently absent.
Contact `gabrielgun1983@gmail.com` or use the public issue tracker.
