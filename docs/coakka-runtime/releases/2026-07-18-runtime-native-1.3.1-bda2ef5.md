# 2026-07-18 Runtime Native 1.3.1 bda2ef5

This note records the current public runtime native version cut to
`1.3.1+bda2ef5`.

| Surface | Artifact | Version |
| --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.1+bda2ef5/coakka-runtime-native-v2-1.3.1.tar.gz` | `1.3.1+bda2ef5` |

This cut moves the runtime-native product line to `1.3.1` and becomes the
native package truth for the current runtime connector family.

Published platform coverage in this package is:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

All published shared libraries in this cut remain self-contained from the user
point of view: the package does not require ambient non-system implementation
libraries or a separate native helper install.

Validation covered:

- Linux `aarch64` and `x86_64` public native package staging in Docker with
  self-contained compiler-runtime linkage
- macOS `aarch64` public native rebuild with loadable `LC_ID_DYLIB`
- Windows `aarch64` and `x86_64` public DLL builds
- native dependency checks and public surface scanning
- deterministic native archive refresh and publish-manifest update
