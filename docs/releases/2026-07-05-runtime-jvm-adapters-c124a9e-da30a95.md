# 2026-07-05 Runtime JVM Adapter c124a9e Refresh da30a95

This note records the Spring Boot and Quarkus adapter refresh over JVM runtime
`0.2.0-gc124a9e-da30a95`.

| Surface | Public artifact | Version | Runtime JVM dependency |
| --- | --- | --- | --- |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `0.2.0-gda30a95` | `0.2.0-gc124a9e-da30a95` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `0.2.0-gda30a95` | `0.2.0-gc124a9e-da30a95` |

The adapter source line now follows connector commit `da30a95`. This refresh
does not introduce a new native package generation; it rebases the published
Spring Boot and Quarkus Maven coordinates onto the current JVM runtime release
line so their manifests and POMs point at the current public runtime artifact.

Validation covered:

- Spring Boot starter jar verification
- Quarkus extension jar verification
- Maven POM and module publication into the static repository layout
- adapter jar manifest `Coakka-Runtime-Jvm-Dependency` parity with the current
  runtime JVM Maven release
- public artifact manifest and surface verification
