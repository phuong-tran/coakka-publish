# Rust Connector Release Notes

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
