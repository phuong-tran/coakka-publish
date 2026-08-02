# Python Connector Release Notes

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
