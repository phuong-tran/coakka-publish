# CoAkka Runtime Zig Connector

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

This is the Zig connector in the polyglot, multi-language, multi-platform CoAkka
runtime ecosystem. CoAkka is not Zig-only: Zig applications can share the same
runtime semantics with native C/C++, JVM, Node/Bun, Python, Go, C#, Rust, Swift,
and Mojo services. It exposes lifecycle, request/reply, diagnostics, capability
discovery, startup connection strategy, structured atomic apply results, and
same-mode TLS/mTLS credential reload over the public C ABI.

Connector version: `2.1.0-source`<br>
Native generation: `2.1.0+60ddf70d`<br>
Publisher signing: absent

## Compatibility

| Target | Native in source tarball | Current evidence |
| --- | --- | --- |
| macOS ARM64 | dylib | Zig request/reply, transport, and TLS/mTLS reload pass |
| Linux ARM64 | ELF shared object | exact payload plus Zig compile/link pass; native execution pending |
| Windows x86-64 | DLL | exact payload plus Zig compile/link pass; native execution pending |

Linux remains the primary runtime deployment target. The distribution includes
Linux, Windows, and macOS even when one connector test run executes on fewer
targets.

## Smoke

```sh
COAKKA_RUNTIME_LIB=/path/to/libcoakka_runtime_v2.dylib \
  bash scripts/smoke.sh
```

`COAKKA_TLS_FIXTURE_ROOT` enables the TLS reload portion and must contain
`ca.pem`, `server.pem`, `server.key`, and a mismatching `client.key`.

The Zig API keeps unknown numeric ABI values representable, sends optional
tuning only when present, and returns `StartHostResult` so startup rejection
does not lose its structured active-state snapshot. See
[transport configuration](TRANSPORT_CONFIGURATION.md).

## Dry-Run Gates

```sh
zig build
bash scripts/check-platform-source.sh
bash scripts/package-release.sh
bash scripts/check-package-readiness.sh
```

These checks build and consume a local source archive without changing any
registry or public release state.

## Common Guidance

- [CoAkka documentation and samples](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
- [Connection strategies](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/connection-strategies.md)
- [TLS/mTLS](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/troubleshooting.md), including Linux loader, Gatekeeper, Windows publisher warnings, checksums, and absent signing
- [Contact/support](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/contact-and-support.md)
- Contact: `gabrielgun1983@gmail.com`

## File Lane

`NativeRuntime.openFileLane(...)` exposes exact file-lane ABI layouts and
explicit shutdown ownership. See the shared [file-lane contract](https://github.com/phuong-tran/coakka-publish/blob/main/docs/runtime-file-transfer.md)
and use a native runtime that exports the complete file-lane symbol set.
