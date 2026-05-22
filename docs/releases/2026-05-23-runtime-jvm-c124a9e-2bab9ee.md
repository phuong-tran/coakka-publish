# 2026-05-23 Runtime JVM c124a9e Rebundle

This note records the JVM runtime artifact rebundle over native package
generation `0.2.0+c124a9e`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `0.2.0-gc124a9e-2bab9ee` | `0.2.0+c124a9e` |

The JVM connector code shape remains the `2bab9ee` source connector line. This
drop refreshes the embedded native libraries and Maven/public release metadata
so the JVM jar no longer bundles `0.2.0+94a5729` runtime binaries while the
public native lane advertises `0.2.0+c124a9e`.

Validation covered:

- JVM packaged-jar content verification
- JVM embedded-native smoke on macOS `aarch64`
- manifest and native-entry hash parity against
  `runtime/native/releases/0.2.0+c124a9e/coakka-runtime-native-v2-0.2.0.tar.gz`
- Maven checksum sidecar verification
- public artifact manifest and surface verification

Python, Node.js, Go, C#, Rust, Spring Boot, and Quarkus runtime packages remain
on their previously declared native package generations until those lanes are
rebundled.
