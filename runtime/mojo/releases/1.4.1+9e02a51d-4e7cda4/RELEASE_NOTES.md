# Mojo Connector Release Notes

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
