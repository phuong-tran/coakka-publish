# Python Runtime Transport Configuration

The public CoAkka C ABI is the semantic authority. The Python connector copies
its results into frozen Python values; it does not reinterpret lifecycle,
capability, validation, or edition behavior.

## Startup Contract

Set `connection_strategy` and `security` on `ConnectorStartSpec` or
`ConnectorConfig`. The connector creates the native handle, applies each
selected policy while the handle is still `CREATED`, installs host handles,
then starts the runtime. A rejected startup apply destroys the handle and raises
`RuntimeTcpConnectionApplyError` or `RuntimeTcpSecurityApplyError`; each error
retains the structured native result in `.result`.

Omit both fields to preserve runtime defaults. Call
`RuntimeHost.read_runtime_capabilities(...)` before selecting an optional mode.
Package names and binary presence do not prove entitlement.

```python
from coakka_v2_connector import (
    ConnectorStartSpec,
    RuntimeCapability,
    RuntimeHost,
    RuntimeTcpConnectionMode,
    RuntimeTcpConnectionStrategySpec,
    RuntimeTcpSecurityMode,
    RuntimeTcpSecuritySpec,
    local_route,
)

library = "/opt/coakka/lib/libcoakka_runtime_v2.so"
capabilities = RuntimeHost.read_runtime_capabilities(library)
mode = (
    RuntimeTcpConnectionMode.BOUNDED_POOL
    if capabilities.supports(RuntimeCapability.TCP_BOUNDED_POOL)
    else RuntimeTcpConnectionMode.PER_EXCHANGE
)

spec = ConnectorStartSpec(
    system_name="orders",
    node_id="orders-1",
    routes=[local_route("orders.create")],
    connection_strategy=RuntimeTcpConnectionStrategySpec(mode=mode),
    security=RuntimeTcpSecuritySpec(mode=RuntimeTcpSecurityMode.PLAINTEXT),
)

with RuntimeHost.start(spec, library) as runtime:
    assert runtime.startup_connection_result().applied()
```

## Public Functions

| Function | Purpose and parameters | Default/result | Ownership, thread safety, blocking, lifecycle, atomicity, errors, edition |
| --- | --- | --- | --- |
| `RuntimeHost.read_runtime_capabilities(runtime_lib_path=None)` | Load a runtime library and read compiled, entitled, and effective capability bits. The path follows normal resolver rules when omitted. | Returns a copied `RuntimeCapabilitiesSnapshot`. | No runtime handle is created. The native call is synchronous and thread-safe. Loader errors propagate. Available in all editions. |
| `capabilities.supports(bits)` | Check whether every requested capability bit is effective. | Returns `True` for a complete bit-set match, including the empty set. | Pure copied-value operation; non-blocking and thread-safe. It does not infer capability from edition or package identity. |
| `RuntimeHost.start(start_spec, runtime_lib_path=None)` | Own one process-local runtime. `connection_strategy` and `security` are optional startup policies. | Omitted policies preserve native defaults. Startup result snapshots are retained. | Applies policy before host handles/start. A rejected apply destroys the new handle and raises a typed error with the native result. Credential file reads can block on local I/O. One active host per process. |
| `runtime.runtime_capabilities()` | Read capability truth from the loaded runtime. | Copied `RuntimeCapabilitiesSnapshot`. | Synchronous; contains no secret. Available before selecting optional features. |
| `runtime.tcp_connection_config()` | Read the effective connection mode and tuning provenance. | Copied `RuntimeTcpConnectionConfigSnapshot`. | Serialized with apply operations on this host. Synchronous and valid only while open. Available in all editions; effective modes/tuning depend on capabilities. |
| `runtime.tcp_security_info()` | Read active mode, generation, credential ID, protocol/verification metadata, validity, and certificate fingerprint. | Copied `RuntimeTcpSecurityInfoSnapshot`. | Serialized with apply operations. It excludes certificate/key paths, PEM bytes, keys, and provider diagnostics. TLS fields depend on edition/capability. |
| `runtime.startup_connection_result()` | Inspect the startup connection apply. | `None` when no explicit connection policy was selected; otherwise copied result. | Non-blocking Python field read. Valid after start and after close. |
| `runtime.startup_security_result()` | Inspect the startup security apply. | `None` when no explicit security policy was selected; otherwise copied result. | Non-blocking Python field read. Valid after start and after close; contains no secret paths/material. |
| `runtime.apply_tcp_connection_strategy(spec)` | Attempt a connection policy apply and return the effective state after the attempt. | Returns `RuntimeTcpConnectionApplyResult`; it does not throw for a native policy rejection. | Per-handle calls are serialized. Native call is synchronous and atomic. The runtime accepts connection changes only in `CREATED`, so a normally started Python host returns structured `ERR_BAD_STATE` and preserves active config. Throws only for closed handle, local conversion error, or ABI inconsistency. |
| `runtime.apply_tcp_security(spec)` | Attempt a security apply or same-mode newer-generation TLS/mTLS reload. | Returns `RuntimeTcpSecurityApplyResult`; it does not throw for a native policy rejection. | Per-handle calls are serialized. Paths are borrowed only for the synchronous call; the native runtime loads/validates credentials before atomic publication. Failed/stale reload keeps active generation. File I/O may block. TLS/mTLS/reload require effective capabilities. |
| `connection_result.applied()` / `security_result.applied()` | Test whether the native apply status is `OK`. | Returns a boolean; `changed` remains a separate fact. | Pure copied-value operation; non-blocking and thread-safe. A successful no-op can be applied without changing state. |

`ConnectorOrchestrator` is the compatibility name for `RuntimeHost` and exposes
the same functions.

## Specs And Defaults

`RuntimeTcpConnectionStrategySpec` defaults to `PER_EXCHANGE`; tuning fields
default to `None`, meaning not explicitly configured. Do not set tuning fields
for modes where the core marks them inapplicable. The core returns stable
validation codes instead of silently discarding them.

`RuntimeTcpSecuritySpec` defaults to `PLAINTEXT`, graceful reload, generation
zero, and empty credential fields. TLS/mTLS requires file-backed CA,
certificate, and private-key paths plus a positive generation and a stable
non-secret credential ID. Supplying credential fields in plaintext mode is an
error, not an ignored configuration.

See the canonical [connection strategy guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md)
for `PER_EXCHANGE`, `BOUNDED_POOL`, `PERSISTENT_SINGLE_FLIGHT`, and
`MULTIPLEXING` defaults and availability.

## TLS Reload

```python
from coakka_v2_connector import RuntimeTcpSecurityMode, RuntimeTcpSecuritySpec

generation_2 = RuntimeTcpSecuritySpec(
    mode=RuntimeTcpSecurityMode.TLS,
    credential_generation=2,
    credential_id="orders-edge-2026-08-b",
    ca_certificate_file="/run/secrets/coakka/ca.pem",
    identity_certificate_file="/run/secrets/coakka/tls.crt",
    private_key_file="/run/secrets/coakka/tls.key",
)
result = runtime.apply_tcp_security(generation_2)
if not result.applied():
    # result.active_security is still the configuration serving traffic.
    raise RuntimeError(
        f"reload rejected: {result.reason_name}; "
        f"active generation={result.active_security.credential_generation}"
    )
```

The connector never returns secret file paths or material in snapshots. The
caller owns path strings and on-disk secret lifecycle. The runtime validates
and adopts credential content during the synchronous apply; a failed reload
does not replace active credentials.

See the canonical [TLS/mTLS guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
for Kubernetes ingress/service-mesh guidance, controlled networks, LAN/edge,
RPi, BeagleBone, bare metal, industrial Android, and rotation semantics.

## Errors And Troubleshooting

Native rejections preserve `status`, stable numeric `reason`, `reason_name`,
validation code/field/range, runtime state, `changed`, and the active snapshot.
Unknown enum values remain numeric so a newer runtime does not crash an older
Python projection.

Use [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for native loading, capability, TLS material, platform trust/signing, and
reload failures. Publisher signing is currently absent and reported separately
from digest and execution evidence. Contact
`gabrielgun1983@gmail.com` or use the public issue tracker for support.
