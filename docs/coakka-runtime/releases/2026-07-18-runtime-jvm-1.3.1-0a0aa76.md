# 2026-07-18 Runtime JVM 1.3.1 Refresh 0a0aa76

This note records the JVM runtime connector refresh over native package
generation `1.3.1+bda2ef5`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.3.1-gbda2ef5-0a0aa76` | `1.3.1+bda2ef5` |

The JVM connector packaging follows connector source commit `0a0aa76` while
rebundling against the current public native package truth. The published jar
keeps the embedded-native loading path and carries the same five-platform
native set as the root public runtime package.

Current embedded-native coverage in the published jar is:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

Validation covered:

- JVM test suite and packaged-jar content verification
- JVM packaged-jar distribution refresh
- packaged-jar embedded runtime smoke reporting `runtime_version=1.3.1`
- Maven checksum sidecar verification
- public artifact manifest and surface verification

Spring Boot and Quarkus adapter dependency refresh over this runtime line is
recorded separately in `2026-07-18-runtime-jvm-adapters-1.3.1-0a0aa76.md`.
