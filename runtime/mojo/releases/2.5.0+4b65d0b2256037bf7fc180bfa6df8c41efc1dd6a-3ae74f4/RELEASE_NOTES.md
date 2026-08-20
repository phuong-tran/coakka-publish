# Mojo Connector Release Notes

## 2.5.2-source

- Adds strict C conformance shims for File and Stream Lane owner grants and
  verifies owner-aware native entry points in package smoke tests.
- Does not claim a stable high-level Mojo owner-grant API; consumers use the
  documented C shim boundary in this source package.
- Keeps native generation
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a` unchanged.

## 2.5.1-source

- Corrects public documentation and adopts the file-scoped Apache-2.0 and
  CoAkka Native Artifact License 1.2 package map.
- Keeps source APIs, native ABI, and the exact native generation unchanged.

## 2.5.0-source

- Adds the File Lane and Stream Lane source surfaces.
- Locks the source package to exact native generation
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.

## 2.4.0-source

- Added explicit embedded, outbound-only, and network-node startup modes to
  the bounded C shim.
- Locked the source package to exact native generation `2.4.0+c2f53117`.

## 2.3.0-source

- Added the bounded Stream Lane C-shim contract and candidate smoke.
- Locked the source package to exact native generation `2.3.0+a83ab412`.

## 2.1.0-source

- Added a standalone C-shim file-lane smoke for receive preparation, send,
  progress, terminal state, stats, SHA-256 equality, and draining shutdown.
- Locked the source package to exact five-platform native generation
  `2.1.0+60ddf70d`.
- Executed the shim on macOS ARM64 and Linux ARM64/x86-64. Mojo language
  execution on Linux remains unclaimed where the guest has no Mojo toolchain.

## 1.4.1-source

- Exact layouts for eleven transport ABI blocks.
- Capability-driven connection policy before host-handle export and start.
- Structured rejection for unknown connection modes, plaintext-only fields,
  and post-start reconfiguration.
- Atomic TLS reload checks that retain the active generation after bad-key and
  stale-generation attempts.
- Cross-platform close and sleep mechanics in the C shim.
- Digest-locked macOS ARM64, Linux ARM64/x86-64, and Windows ARM64/x86-64 native payloads.

macOS ARM64 request/reply, deadletter, transport policy, and TLS/mTLS
reload are exercised by the packaged consumer smoke. Linux ARM64 and Windows
x86-64 C shim compilation and exact native payload checks are included as
separate evidence.

See [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).

Contact: `gabrielgun1983@gmail.com`.
