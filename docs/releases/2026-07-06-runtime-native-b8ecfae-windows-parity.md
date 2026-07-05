# 2026-07-06 Runtime Native b8ecfae Windows Parity

This note records the native runtime `0.2.0+b8ecfae` public refresh after the
Windows package surface was widened to full parity with the current macOS/Linux
train.

| Surface | Artifact | Version |
| --- | --- | --- |
| Native C ABI | `runtime/native/releases/0.2.0+b8ecfae/coakka-runtime-native-v2-0.2.0.tar.gz` | `0.2.0+b8ecfae` |

The refresh keeps the public runtime surface self-contained while moving the
current public archive to a five-platform native set:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

All published shared libraries in this cut are dependency-clean from the user
point of view: the current package does not require ambient `protobuf`, `absl`,
`CAF`, `libuv`, `libstdc++`, `libgcc`, or `winpthread` runtime installs.

Validation covered:

- Linux `aarch64` and `x86_64` public native package staging
- macOS `aarch64` public native rebuild with loadable `LC_ID_DYLIB`
- Windows `aarch64` public DLL build plus real Windows guest host-runtime proof
- Windows `x86_64` public DLL build plus real Windows guest host-runtime proof
  through the x64 emulation path on Windows 11 ARM
- native dependency checks and public surface scanning
- deterministic native archive refresh and publish-manifest update
