# C# Connector Release Notes

## 1.4.0

This release adds runtime-v2 transport configuration while preserving the
public C ABI as semantic authority:

- exact P/Invoke layouts for all eleven capability, connection, security, and
  structured-result blocks
- capability discovery before selecting optional modes
- startup connection strategy and TLS/mTLS policy before handle export/start
- copied structured rejection results and active-state preservation
- same-mode newer-generation TLS/mTLS credential reload
- process-lifetime native-library identity across sequential host lifecycles
- serialization of transport getters/applies with host disposal
- OS-specific frame-reader calls: POSIX nonblocking `read` on macOS/Linux and
  `PeekNamedPipe` before Universal CRT `_read` on Windows
- exact-once close of every host-owned exported fd on normal disposal and on
  startup failure after handle export

Baseline and exact full-capability conformance smokes pass on macOS ARM64; the
full-capability build includes rejected key mismatch preservation and successful
generation reload. The NuGet package verifier requires exact release-metadata digests
for macOS ARM64, Linux ARM64, and Windows x86-64. Linux/Windows payload presence
and cross-publish selection are not connector execution evidence; this package
receipt makes no C# connector or consumer execution claim for those targets.

Publisher signing is absent. Platform trust and checksum verification are
explained in [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).

Contact: `gabrielgun1983@gmail.com` or the public issue tracker.
