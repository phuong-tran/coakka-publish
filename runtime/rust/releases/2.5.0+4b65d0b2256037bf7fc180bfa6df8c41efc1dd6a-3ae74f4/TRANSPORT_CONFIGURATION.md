# Rust Runtime Transport Configuration

The public CoAkka C ABI remains the semantic authority. The Rust connector
copies capability, configuration, validation, apply-result, and TLS identity
data into owned Rust values. It never retains native snapshot pointers or
returns credential paths, PEM material, private keys, or provider tokens.

## Startup

Set `connection_strategy` and `security` on `ConnectorStartSpec`. The connector
creates the runtime, applies selected policy while it is `CREATED`, exports host
handles, applies routes, and starts. `None` preserves runtime defaults. A
rejected startup apply destroys the native runtime and returns
`CoAkkaError::TcpConnectionApply` or `CoAkkaError::TcpSecurityApply` with the
boxed, copied structured result.

```rust
let capabilities = RuntimeHost::read_runtime_capabilities("")?;
let mode = if capabilities
    .effective_capabilities
    .supports(RuntimeCapabilities::TCP_BOUNDED_POOL)
{
    TcpConnectionMode::BOUNDED_POOL
} else {
    TcpConnectionMode::PER_EXCHANGE
};

let mut spec = ConnectorStartSpec::new("orders", "orders-1");
spec.connection_strategy = Some(TcpConnectionStrategySpec::new(mode));
spec.security = Some(TcpSecuritySpec::new(TcpSecurityMode::PLAINTEXT));
let runtime = RuntimeHost::start(spec)?;
```

## Public Functions

| Function | Purpose, parameters, defaults | Ownership, thread safety, blocking, lifecycle, atomicity, errors, edition |
| --- | --- | --- |
| `RuntimeHost::read_runtime_capabilities(path)` | Resolves one runtime library; empty path uses resolver order. | Synchronous and process-safe. The first module identity stays loaded; a different later path is rejected. All editions. |
| `capabilities.supports(bits)` | Requires every requested effective bit. `NONE` is true. | Pure copied-value operation; never infers support from a profile or package name. |
| `TcpConnectionStrategySpec::new(mode)` | Selects a mode and leaves all tuning absent. | Caller owns the value. Optional tuning is only sent when `Some`; core validates applicability and edition/capability. |
| `TcpSecuritySpec::new(mode)` | Selects a mode with graceful reload, generation zero, and empty credentials. | Caller owns strings and secret-file lifecycle. TLS/mTLS requires explicit file fields, ID, and positive generation. |
| `RuntimeHost::start` / `start_with_runtime_lib` | Starts one host; optional transport specs preserve defaults when absent. | Policy applies before handle export/start. Credential loading may block. Startup rejection returns a structured error and releases runtime/fd ownership. |
| `runtime.runtime_capabilities()` | Copies capability truth through the active host. | Serialized with transport apply; open host only; all editions. |
| `runtime.tcp_connection_config()` | Copies mode, tuning, and explicit/default provenance. | Synchronous, serialized, open host only; optional modes follow effective capabilities. |
| `runtime.tcp_security_info()` | Copies active non-secret TLS/mTLS state and certificate identity. | Synchronous and serialized. No secret/path borrowing escapes the call; TLS fields are capability-gated. |
| `runtime.startup_tcp_connection_result()` | Returns the explicit startup result. | `None` when omitted; returns an owned clone that remains valid independently of the host. |
| `runtime.startup_tcp_security_result()` | Returns the explicit startup result. | `None` when omitted; returns an owned non-secret clone. |
| `runtime.apply_tcp_connection_strategy(spec)` | Attempts an atomic connection apply. | Serialized. Connection mode is startup-only, so a started host normally returns `BAD_STATE` and the unchanged active config. |
| `runtime.apply_tcp_security(spec)` | Applies startup-shaped security or same-mode newer-generation reload. | Strings are borrowed only for the synchronous call; file I/O may block. Invalid/stale reload preserves the active immutable context. |
| `result.applied()` | Tests whether native status is `OK`. | Pure copied-value operation; successful no-op and `changed == false` are distinct. |
| `RuntimeTransportAbi::sizes()` | Reports Rust sizes for eleven C ABI blocks. | Layout diagnostic only. It does not prove runtime execution on another OS or CPU. |

All transport methods preserve unknown numeric C ABI values in transparent Rust
newtypes. The binding does not remap an unsupported request into another mode.

## TLS Reload

```rust
let mut reload = TcpSecuritySpec::new(TcpSecurityMode::TLS);
reload.credential_generation = 2;
reload.credential_id = "orders-2026-08".into();
reload.ca_certificate_file = "/run/secrets/coakka/ca.pem".into();
reload.identity_certificate_file = "/run/secrets/coakka/tls.pem".into();
reload.private_key_file = "/run/secrets/coakka/tls.key".into();
let result = runtime.apply_tcp_security(&reload)?;
```

The runtime loads and validates the files synchronously, then atomically
publishes a private immutable TLS context. A rejected mismatch or stale
generation leaves the previous context active. See the canonical
[TLS/mTLS guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
for Kubernetes ingress/service mesh, controlled networks, LAN/edge, RPi,
BeagleBone, bare metal, industrial Android, and rotation semantics.

## Platforms And Troubleshooting

The staged tarball digest-verifies and includes macOS ARM64, Linux ARM64, and
Windows x86-64. Payload presence and cross-platform source build are not native
execution evidence. Use [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for loader/CPU mismatch, dependencies, certificates, Gatekeeper, Authenticode,
checksums, and the currently absent publisher signature. Signing is deferred
and is not a connector gate. Contact `gabrielgun1983@gmail.com` or the public
issue tracker.
