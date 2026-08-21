# Zig Runtime Transport Configuration

The public C ABI is the semantic authority. Zig copies capability, validation,
effective configuration, and non-secret TLS identity into owned values. Raw
numeric wrappers preserve values added by a newer runtime.

## Startup

Set optional `connection_strategy` and `security` on `StartSpec`. Null keeps
core defaults. Explicit policy is applied while `CREATED`, before host-handle
export and start. `StartHostResult` contains either the host or the structured
connection/security rejection.

```zig
const capabilities = try native.readCapabilities();
const mode: runtime.TcpConnectionMode = if (
    capabilities.effective.supports(.tcp_bounded_pool)
) .bounded_pool else .per_exchange;

var spec = runtime.localStartSpec("orders", "orders-1", "svc.orders");
spec.connection_strategy = .{ .mode = mode };
spec.security = .{};
var host = switch (try native.startHost(spec)) {
    .started => |started| started,
    .connection_rejected, .security_rejected => return error.StartupRejected,
};
defer host.deinit();
```

## Public Functions

| Function | Contract |
| --- | --- |
| `NativeRuntime.open(path)` | Synchronously loads one exact module path and resolves required symbols. Thread-safe; first identity stays process-loaded; a different path is rejected. All editions; returns loader/symbol errors. |
| `NativeRuntime.close()` | Releases the Zig value but intentionally does not unload process-native state. No blocking or mutation of active runtime state. |
| `readInfo()` | Copies ABI/build pointers for immediate inspection and verifies ABI 1. Synchronous; all editions. |
| `readCapabilities()` | Copies compiled, entitled, and effective capability truth. Synchronous; all editions; no package-name inference. |
| `startHost(spec)` | Reserves the single process host, creates runtime, atomically applies explicit transport, applies routes, exports handles, and starts. Credential file I/O may block. Failure releases runtime/fd ownership. |
| `capabilities()` | Copies module capability truth through an open host; serialized with apply and teardown. |
| `connectionConfig()` | Copies effective strategy, tuning, and explicit/default provenance. Synchronous, serialized, open host only. |
| `securityInfo()` | Copies active non-secret TLS/mTLS generation and certificate identity. Never returns paths, PEM, keys, or tokens. |
| `applyConnectionStrategy(spec)` | Atomic synchronous attempt. Nil tuning remains absent. After start normally returns `bad_state`, `changed == false`, and unchanged active config. |
| `applySecurity(spec)` | Atomic synchronous same-mode newer-generation reload. Sentinel strings are borrowed for the call; file loading may block. Invalid/stale credentials preserve active state. |
| `deinit()` | Idempotently stops, closes each unique exported fd once, destroys runtime, and releases the process host slot; serialized with transport calls. |
| `transport.abiSizes()` | Reports eleven target-compiled ABI sizes. Diagnostic only, not execution evidence. |

Connection modes are `.per_exchange`, `.bounded_pool`,
`.persistent_single_flight`, and `.multiplexing`. Optional tuning availability
and defaults come from capability/config snapshots and the canonical
[connection guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md).

## TLS Reload

The caller owns credential files and their permissions/lifetime. Embedded NUL
is rejected locally. The runtime synchronously loads and validates all files,
then atomically publishes a private immutable TLS context. Rejection keeps the
previous generation and fingerprint. See the canonical
[TLS/mTLS guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md).

## Platforms And Signing

The source archive includes exact macOS ARM64, Linux ARM64/x86-64, and Windows ARM64/x86-64
candidate natives. Publisher signing is absent/deferred and is not a connector
gate. See [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for platform trust warnings, checksums, loader dependencies, CPU mismatch, and
certificate diagnostics.
