# 2026-05-23 Runtime JVM Adapter c124a9e Rebundle

This note records the Spring Boot and Quarkus adapter rebundle over JVM runtime
`0.2.0-gc124a9e-2bab9ee`.

| Surface | Public artifact | Version | Runtime JVM dependency |
| --- | --- | --- | --- |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `0.2.0-g1c2694b` | `0.2.0-gc124a9e-2bab9ee` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `0.2.0-g1c2694b` | `0.2.0-gc124a9e-2bab9ee` |

The adapter source line is connector commit `1c2694b`, which updates the
framework adapter defaults to the JVM runtime jar rebundled with native package
generation `0.2.0+c124a9e`.

Validation covered:

- Spring Boot starter jar verification
- Quarkus extension jar verification
- Maven POM and module publication into the static repository layout
- adapter jar manifest `Coakka-Runtime-Jvm-Dependency` parity with the current
  runtime JVM Maven release
- public artifact manifest and surface verification
