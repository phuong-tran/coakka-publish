# 2026-07-18 Runtime JVM Adapters 1.3.1 Refresh 0a0aa76

This note records the Spring Boot and Quarkus adapter refresh over JVM runtime
`1.3.1-gbda2ef5-0a0aa76`.

| Surface | Public artifact | Version | Runtime JVM dependency |
| --- | --- | --- | --- |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.3.1-g0a0aa76` | `1.3.1-gbda2ef5-0a0aa76` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.3.1-g0a0aa76` | `1.3.1-gbda2ef5-0a0aa76` |

The adapter source line follows connector source commit `0a0aa76` while
rebasing the published Spring Boot and Quarkus Maven coordinates onto the
current `1.3.1` runtime line. This cut keeps adapter dependencies aligned with
the refreshed runtime JVM package instead of mixing old and new runtime trains.

Validation covered:

- Spring Boot starter jar verification
- Quarkus extension jar verification
- Maven POM and module publication into the static repository layout
- adapter jar manifest `Coakka-Runtime-Jvm-Dependency` parity with the current
  runtime JVM Maven release
- public artifact manifest and surface verification
