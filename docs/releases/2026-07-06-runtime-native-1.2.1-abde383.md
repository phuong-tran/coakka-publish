# 2026-07-06 Runtime Native 1.2.1 abde383

This note records the current public runtime native version cut to
`1.2.1+abde383`.

| Surface | Artifact | Version |
| --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.2.1+abde383/coakka-runtime-native-v2-1.2.1.tar.gz` | `1.2.1+abde383` |

This cut keeps the existing five-platform native package truth while moving the
public product/runtime line to `1.2.1`.

Published platform coverage in this package is:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

All published shared libraries in this cut remain self-contained from the user
point of view: the package does not require ambient `protobuf`, `absl`, `CAF`,
`libuv`, `libstdc++`, `libgcc`, `winpthread`, or a separate native helper
install.

Validation covered:

- Linux `aarch64` and `x86_64` public native package staging in Docker with
  static `libstdc++` and `libgcc`
- macOS `aarch64` public native rebuild with loadable `LC_ID_DYLIB`
- Windows `aarch64` and `x86_64` public DLL builds
- native dependency checks and public surface scanning
- deterministic native archive refresh and publish-manifest update
