# Zig Connector Release Notes

## 2.4.0-source

- Added explicit embedded, outbound-only, and network-node startup modes.
- Kept local modes listener-free and network-node bind failure fail-closed.
- Locked the source package to exact native generation `2.4.0+c2f53117`.

## 2.3.0-source

- Added exact Stream Lane ABI assertions, bounded callbacks, explicit session
  ownership, pressure snapshots, and candidate smoke.
- Locked the source package to exact native generation `2.3.0+a83ab412`.

## 2.1.0-source

- Added direct file-lane ABI bindings with typed configuration, receive/send
  specs, SHA-256, sequence waits, cancellation, terminal cleanup, stats, and
  explicit stop-before-drain close.
- Locked the source archive to native generation `2.1.0+60ddf70d` across Linux
  ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64.
- Passed package readiness, clean macOS consumer execution, a multi-quantum
  macOS file transfer, native Linux ARM64/x86-64 file transfers, and strict
  Windows x86-64 compile/link coverage.

## 1.4.1-source

- Exact layouts for eleven transport ABI blocks and unknown-value preservation.
- Capability-driven policy before handle export/start with structured startup rejection.
- Atomic same-mode TLS reload with bad-key and stale-generation preservation.
- Process-lifetime native identity, single active host, and duplicate-safe fd cleanup.
- Cross-platform C loader/close/sleep mechanics for macOS, Linux, and Windows.
- Exact five-native source tarball with checksum and signing metadata.

macOS ARM64 request/reply, deadletter, transport, and TLS/mTLS reload
pass. Linux ARM64 and Windows x86-64 compile/link and exact payload checks pass;
native execution on those targets is not yet recorded.

Publisher signing is absent. See [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).

Contact: `gabrielgun1983@gmail.com`.
