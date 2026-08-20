# Rust Connector Release Notes

## 2.5.2

Adds typed File and Stream Lane owner grants, validated control-plane
reconstruction, redacted secret-bearing debug output, exact-owner endpoint
pinning, and live owner-aware tests. Native generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a` is unchanged.

## 2.5.1

Corrects public documentation and adopts the file-scoped Apache-2.0 and CoAkka
Native Artifact License 1.2 package map. Rust APIs, native ABI, and the exact
native generation are unchanged.

## 2.5.0

Adds File Lane and Stream Lane and embeds all five payloads from exact native
generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.

## 2.4.0

Adds explicit embedded, outbound-only, and network-node startup modes over
exact native generation `2.4.0+c2f53117`. The Rust and trusted Tauri host
boundaries preserve no-listener behavior for local modes and fail-closed bind
ownership for network nodes.

## 2.3.0

Adds documented Stream Lane ownership and checked projections for Rust and the
trusted Tauri host boundary. The source archive carries exact five-platform
native generation `2.3.0+a83ab412`.

## 2.1.0

This source release adds a `Send + Sync` `FileLane` over native generation
`2.1.0+60ddf70d`. It exposes typed receive/send specs, SHA-256, progress waits,
cancellation, terminal-record cleanup, stats, and draining `Drop` ownership.
Checked enum projection rejects unknown native values rather than silently
misclassifying transfer state.

The archive contains the exact five-platform runtime matrix. Package
readiness, clean source-consumer smoke, macOS ARM64 file transfer, and Linux
ARM64/x86-64 request/reply plus file-transfer execution pass. The Linux loader
uses target-correct `RTLD_LOCAL`; the historical 1.4.1 loader defect is not
carried into 2.1.0.

## 1.4.1

This update adds runtime-v2 transport configuration while preserving the
public C ABI as semantic authority:

- exact Rust layouts for all eleven capability, connection, security, and
  structured-result blocks
- capability discovery, startup policy before handle export/start, and copied
  structured rejection results
- boxed deadletter and transport-apply error payloads so ordinary `Result<T>`
  values do not carry the size of the largest diagnostic snapshot
- atomic same-mode newer-generation TLS/mTLS credential reload with failure
  preservation
- process-lifetime native module identity across sequential host lifecycles
- OS-specific loading and fd I/O for macOS, Linux, and Windows
- `PeekNamedPipe` before Universal CRT `_read` so an idle Windows reader does
  not block teardown
- one close for every unique host-owned exported fd on normal disposal and
  startup failure after handle export

Baseline and full-capability conformance smokes pass on macOS ARM64. The
full-capability checks include rejected key mismatch preservation and successful
generation reload. The source tarball requires exact native digests
for macOS ARM64, Linux ARM64/x86-64, and Windows ARM64/x86-64. Linux ARM64 and Windows x86-64
cross-target source checks pass. Payload presence or source compilation is not
connector execution evidence; Linux/Windows native runtime and consumer
execution is not yet recorded.

Publisher signing is absent. Platform trust is covered by
[common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).

Contact: `gabrielgun1983@gmail.com` or the public issue tracker.
