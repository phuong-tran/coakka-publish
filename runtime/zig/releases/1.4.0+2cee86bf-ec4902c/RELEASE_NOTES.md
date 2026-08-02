# Zig Connector Release Notes

## 1.4.0-source

- Exact layouts for eleven transport ABI blocks and unknown-value preservation.
- Capability-driven policy before handle export/start with structured startup rejection.
- Atomic same-mode TLS reload with bad-key and stale-generation preservation.
- Process-lifetime native identity, single active host, and duplicate-safe fd cleanup.
- Cross-platform C loader/close/sleep mechanics for macOS, Linux, and Windows.
- Exact three-native source tarball with checksum and signing metadata.

macOS ARM64 request/reply, deadletter, transport, and TLS/mTLS reload
pass. Linux ARM64 and Windows x86-64 compile/link and exact payload checks pass;
native execution on those targets is not yet recorded.

Publisher signing is absent. See [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).

Contact: `gabrielgun1983@gmail.com`.
