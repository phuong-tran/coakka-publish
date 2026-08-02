# C# Connector Release Notes

## 1.4.6

This NuGet-only patch makes the package README compatible with the NuGet.org
CommonMark renderer. The logo now uses Markdown image syntax from a NuGet
allow-listed HTTPS host, and C# transport/package documentation links use
absolute public URLs instead of package-relative paths.

Runtime behavior, public ABI, native generation `1.4.0+2cee86bf`, the three
RID payloads, capabilities, license, and publisher-signing state are unchanged.

## 1.4.5

This NuGet package release carries the C# connector contract introduced in the
1.4.0 artifact train while preserving the exact native runtime generation
`1.4.0+2cee86bf`. The package version advances independently so the immutable
NuGet coordinate does not imply a native rebuild or a public ABI change.

The package embeds the verified macOS ARM64, Linux ARM64, and Windows x86-64
native payloads from the public 1.4.0 release, includes CoAkka Public Artifact
License 1.1, and keeps platform execution claims separate from packaged binary
presence.

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
