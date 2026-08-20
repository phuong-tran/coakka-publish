# Python Connector Release Notes

## 2.5.1

- Corrects registry documentation and adopts the file-scoped Apache-2.0 and
  CoAkka Native Artifact License 1.2 package map.
- Keeps Python APIs, native ABI, and the exact native generation unchanged.

## 2.5.0

- Adds the public File Lane and Stream Lane connector surfaces.
- Bundles all five platform payloads from exact native generation
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.

## 2.4.0

Adds explicit embedded, outbound-only, and network-node startup modes over
exact native generation `2.4.0+c2f53117`. Non-listening modes report local
port `0`; network nodes require explicit bind and advertise endpoints and fail
closed on bind errors.

## 2.3.0

Adds typed Stream Lane publish/subscribe, copied public snapshots, explicit
millisecond waits, pressure and credit signals, and exact five-platform native
generation `2.3.0+a83ab412`.

## 2.1.0

This release adds the `FileLane` bulk-transfer API and embeds exact native
generation `2.1.0+60ddf70d` for Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64. The wheel gate requires both generic and immutable native names
for every platform and rejects any digest or identity mismatch.

`FileLane` supports bounded receive admission, sender submission, SHA-256,
sequence-based progress waits, cancellation, retained terminal records, stats,
and context-manager close. Direct TCP, TLS, and mutual TLS are startup
profiles. Transfer tokens, local paths, and TLS secrets are not returned in
snapshots. The release-candidate wheel installs in a clean environment, and
the connector executes a `9 MiB + 731 byte` transfer with SHA-256 equality on
macOS ARM64.

## 1.4.6

This corrective PyPI release embeds native runtime generation
`1.4.1+9e02a51d` for the exact five-platform production matrix: Linux
ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64. Staging rejects a missing,
additional, duplicated, identity-mismatched, or checksum-mismatched native.
Wheel readiness independently requires exactly those five platform directories
and verifies both the generic and immutable native copy for every platform.

Python connector execution is recorded on macOS ARM64. Native format,
architecture, exports, and checksums are verified for every payload, but wheel
presence is not a Linux or Windows connector-execution claim. Publisher signing
is absent.

## 1.4.5

This PyPI package release carries the Python connector contract introduced in
the 1.4.0 artifact train while preserving the exact native runtime generation
`1.4.0+2cee86bf`. The package version advances independently so the immutable
PyPI coordinate does not imply a native rebuild or an ABI change.

The wheel embeds the verified macOS ARM64, Linux ARM64, and Windows x86-64
native payloads from the public 1.4.0 release, includes CoAkka Public Artifact
License 1.1, and keeps platform execution claims separate from packaged binary
presence.

## 1.4.0

This release adds runtime-v2 transport configuration without changing native
semantics:

- exact 64-bit `ctypes` layouts for capability, connection, security, and
  structured apply-result ABI blocks
- capability discovery before optional feature selection
- startup-configured connection strategy and TLS/mTLS policy
- atomic result projection with active-state preservation after rejection
- same-mode newer-generation TLS/mTLS credential reload
- copied non-secret security identity snapshots
- stable typed startup errors retaining the native structured result

macOS ARM64 source tests pass against the baseline capability profile and the
exact full-capability runtime generation `1.4.0+2cee86bf`. The full-capability
build also proves rejected key mismatch preservation and successful generation
reload.
A host-only wheel installs in a clean environment and completes capability,
startup-policy, snapshot, and request/reply consumer checks.

The wheel includes native payloads for macOS ARM64, Linux ARM64, and Windows
x86-64. Exact platform execution claims are reported separately from payload
presence. Platform loader and publisher-trust behavior is covered by common
troubleshooting.

This package receipt makes no Python execution claim for Linux or Windows;
packaged payload presence does not substitute for that evidence. Publisher
signing is absent.

Canonical guides: [connection strategies](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md),
[TLS/mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md),
[troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md),
and [contact/support](https://github.com/phuong-tran/coakka-publish/blob/main/docs/contact-and-support.md).
