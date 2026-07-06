# 2026-07-06 Runtime JVM 1.2.1 Refresh fa29f94

This note records the JVM runtime connector refresh over native package
generation `1.2.1+abde383`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.2.1-gabde383-fa29f94` | `1.2.1+abde383` |

The JVM connector packaging now follows connector source commit `fa29f94`
while rebundling against the current public native package truth. The
published jar keeps the embedded-native loading path and carries the same
five-platform native set as the root public runtime package.

Current embedded-native coverage in the published jar is:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

Validation covered:

- JVM test suite and packaged-jar content verification
- JVM packaged-jar distribution refresh
- manifest and native-entry hash parity against
  `runtime/native/releases/1.2.1+abde383/coakka-runtime-native-v2-1.2.1.tar.gz`
- Maven checksum sidecar verification
- public artifact manifest and surface verification

Spring Boot and Quarkus adapter dependency refresh over this runtime line is
recorded separately in `2026-07-06-runtime-jvm-adapters-1.2.1-fa29f94.md`.
