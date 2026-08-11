# Mojo Runtime Transport Configuration

The public C ABI is the semantic authority. The Mojo lane currently calls a
sample-local C shim because Mojo's direct FFI surface is evolving. The shim is
package smoke infrastructure, not a stable application API.

## Lifecycle

The shim discovers runtime capabilities, applies connection and security
policy while the runtime is `CREATED`, applies routes, exports host handles,
and then starts the runtime. Connection strategy changes after start return a
structured `BAD_STATE` result with the active configuration unchanged.

The smoke chooses `BOUNDED_POOL` when the effective capability is available;
otherwise it keeps the core `PER_EXCHANGE` default. All defaults and tuning
availability come from runtime capability/config snapshots. See the canonical
[connection strategy guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md).

## TLS Reload

`COAKKA_TLS_FIXTURE_ROOT` enables TLS reload checks. The directory must contain
`ca.pem`, `server.pem`, `server.key`, and a mismatching `client.key`. The
runtime synchronously reads and validates credentials. A bad-key or stale
generation result retains the previously active generation and certificate
identity. Secret paths and bytes are not returned in public snapshots.

The caller owns credential files, permissions, and rotation scheduling. See
the canonical [TLS/mTLS guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md).

## Platforms And Signing

The source distribution includes macOS ARM64, Linux ARM64/x86-64, and Windows ARM64/x86-64
runtime libraries. Publisher signing is currently absent and does not change
the checksum or ABI validation performed by the package scripts. See
[common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for Linux loader dependencies, Gatekeeper, Windows publisher warnings, CPU
architecture mismatch, checksums, and certificate diagnostics.
