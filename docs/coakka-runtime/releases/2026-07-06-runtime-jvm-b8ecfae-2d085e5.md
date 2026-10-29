# 2026-07-06 Runtime JVM b8ecfae Refresh 2d085e5

This note records the JVM runtime connector refresh over native package
generation `0.2.0+b8ecfae`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `0.2.0-gb8ecfae-2d085e5` | `0.2.0+b8ecfae` |

The JVM connector packaging now follows connector source commit `2d085e5`
while rebundling against the current public native package truth. The
published jar keeps the embedded-native loading path and now carries the same
five-platform native set as the root public runtime package.

Current embedded-native coverage in the published jar is:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

Validation covered:

- JVM packaged-jar content verification
- JVM embedded-native smoke on macOS `aarch64`
- Windows 11 ARM guest smoke through the packaged jar default
  embedded-native path for both `windows-aarch64` and `windows-x86_64`
- manifest and native-entry hash parity against
  `runtime/native/releases/0.2.0+b8ecfae/coakka-runtime-native-v2-0.2.0.tar.gz`
- Maven checksum sidecar verification
- public artifact manifest and surface verification

Spring Boot and Quarkus adapter dependency refresh over this runtime line is
recorded separately in `2026-07-06-runtime-jvm-adapters-b8ecfae-2d085e5.md`.
