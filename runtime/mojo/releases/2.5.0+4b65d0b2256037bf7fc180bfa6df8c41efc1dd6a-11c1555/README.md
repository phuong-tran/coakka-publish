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

Version: `2.5.1-source`<br>
Native generation: `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`<br>
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
- [Connection strategies](https://github.com/phuong-tran/coakka-samples/blob/main/docs/connection-strategies.md)
- [TLS/mTLS](https://github.com/phuong-tran/coakka-samples/blob/main/docs/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakka-samples/blob/main/docs/troubleshooting.md), including Linux loader checks, Gatekeeper, Windows publisher warnings, checksums, and signing status
- [Contact/support](https://github.com/phuong-tran/coakka-samples/blob/main/docs/contact-and-support.md)
- Contact: `gabrielgun1983@gmail.com`

## File Lane

`scripts/file-lane-smoke.sh` runs the standalone Mojo/C-shim round trip. This
is exploratory smoke infrastructure, not a stable application API. See the
shared [file-lane contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-file-transfer.md).

## Stream Lane

Version `2.5.1` contains the Stream Lane connector surface over exact native
generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. Keep that pairing intact and follow the public
[streaming contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-streaming.md).

## AI-Assisted Integration

Before generating application code, use the selected connector README together
with the public [AI-assisted integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ai-assisted-integration.md).
It requires an exact package coordinate, platform evidence, the runnable
language sample, and the feature-specific lifecycle contract. Do not translate
API identifiers from another language by analogy.


## License

**Free for application use, including commercial and production use.**

Connector source, generated bindings, type declarations, examples, and package
documentation use the [Apache License, Version 2.0](https://github.com/phuong-tran/coakka-samples/blob/main/LICENSE).
Bundled Native Core files use the [CoAkka Native Artifact License 1.2](https://github.com/phuong-tran/coakka-samples/blob/main/NATIVE-LICENSE.md).
Those native terms permit ordinary application and SaaS use but require a
separate agreement to sell or offer CoAkka itself as managed runtime or
infrastructure.

See [CoAkka Package Licensing](https://github.com/phuong-tran/coakka-samples/blob/main/docs/package-licensing.md)
for the file-scope map. The package also carries offline `LICENSE`,
`NATIVE-LICENSE.md`, `PACKAGE-LICENSE.md`, and `NOTICE` copies.
