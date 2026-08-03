# CoAkka Runtime Mojo Connector

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

This is the Mojo integration in the polyglot, multi-language, multi-platform
CoAkka runtime ecosystem. CoAkka is not Mojo-only: Mojo applications can share
runtime semantics with native C/C++, JVM, Node/Bun, Python, Go, C#, Rust, Swift,
and Zig services. The source connector exercises lifecycle, request/reply,
deadletter delivery, capability discovery, startup connection strategy,
structured apply results, and atomic TLS credential reload through a
sample-local C shim.

Version: `1.4.1-source`<br>
Native generation: `1.4.1+9e02a51d`<br>
Publisher signing: absent

Mojo's direct FFI surface is evolving, so the exported shim function is smoke
infrastructure rather than a stable application API.

## Compatibility

| Target | Included native | Current evidence |
| --- | --- | --- |
| macOS ARM64 | dylib | Mojo request/reply, transport policy, and TLS/mTLS reload pass |
| Linux ARM64 | ELF shared object | exact payload and strict C shim cross-compile pass |
| Windows x86-64 | DLL | exact payload and strict C shim cross-compile pass |

The distribution includes Linux, macOS, and Windows artifacts. Each evidence
entry distinguishes native inclusion, source compilation, and end-to-end Mojo
execution.

## Smoke

```sh
COAKKA_RUNTIME_LIB=/path/to/libcoakka_runtime_v2.dylib \
  bash scripts/smoke.sh
```

Set `COAKKA_TLS_FIXTURE_ROOT` to a directory containing `ca.pem`, `server.pem`,
`server.key`, and a mismatching `client.key` to exercise failed and successful
TLS generation changes.

## Package Checks

```sh
bash scripts/check-platform-source.sh
COAKKA_RUNTIME_STAGING_ROOT=/path/to/exact/native/staging \
  bash scripts/check-package-readiness.sh
```

## Common Guidance

- [CoAkka documentation and samples](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
- [Connection strategies](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/connection-strategies.md)
- [TLS/mTLS](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/troubleshooting.md), including Linux loader checks, Gatekeeper, Windows publisher warnings, checksums, and signing status
- [Contact/support](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/contact-and-support.md)
- Contact: `gabrielgun1983@gmail.com`
