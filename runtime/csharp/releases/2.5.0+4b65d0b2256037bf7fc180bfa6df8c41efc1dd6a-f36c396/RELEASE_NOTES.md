# C# Connector Release Notes

## 2.5.0

Adds File Lane and Stream Lane on .NET 8 and bundles all five payloads from
exact native generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.

## 2.4.1

Lowers the application-host baseline from .NET 10 to .NET 8 and verifies the
same package in .NET 8, .NET 9, and .NET 10 applications. The public managed
API, native ABI, runtime behavior, and exact five-platform native generation
`2.4.0+c2f53117` are unchanged.

## 2.4.0

Adds explicit embedded, outbound-only, and network-node startup modes over
exact native generation `2.4.0+c2f53117`. Embedded and outbound-only hosts do
not open a listener. Network nodes require explicit bind and advertise
endpoints and fail closed on bind errors.

## 2.3.0

Adds `StreamLane` with rooted delegates, borrowed callback spans, copied
snapshots, serialized disposal, and exact native generation
`2.3.0+a83ab412` in all five RID payloads.

## 2.1.0

This release adds `FileLane` as an `IDisposable` bulk-transfer resource over
exact native generation `2.1.0+60ddf70d`. The NuGet package contains Linux
ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64 RID assets and rejects an
incomplete or checksum-mismatched native set before packing.

The typed .NET API covers SHA-256, receive preparation, send submission,
blocking progress waits, cancellation, retained terminal records, bounded
stats, and stop-before-drain disposal. Direct TCP, TLS, and mutual TLS are
explicit configurations. Blocking waits belong off UI threads. The packaged
surface gate and a `9 MiB + 731 byte` native loopback transfer pass on macOS
ARM64.

## 1.4.7

This corrective NuGet release embeds native runtime generation
`1.4.1+9e02a51d` for the exact five-platform production matrix: Linux
ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64. Staging rejects a missing,
additional, duplicated, identity-mismatched, or checksum-mismatched native;
package readiness independently requires exactly five RID assets.

Connector execution is recorded on macOS ARM64. Native format, architecture,
exports, and checksums are verified for every payload, but package presence is
not a Linux or Windows connector-execution claim. Publisher signing is absent.

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
