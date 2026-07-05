# 2026-07-06 Runtime JVM Adapter b8ecfae Refresh 2d085e5

This note records the Spring Boot and Quarkus adapter refresh over JVM runtime
`0.2.0-gb8ecfae-2d085e5`.

| Surface | Public artifact | Version | Runtime JVM dependency |
| --- | --- | --- | --- |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `0.2.0-g2d085e5923d9` | `0.2.0-gb8ecfae-2d085e5` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `0.2.0-g2d085e5923d9` | `0.2.0-gb8ecfae-2d085e5` |

The adapter source line now follows the current connector workspace head while
rebasing the published Spring Boot and Quarkus Maven coordinates onto the
Windows-inclusive `b8ecfae` JVM runtime refresh. This slice closes the publish
verifier dependency chain for the full runtime family rather than only the JVM
lane in isolation.

Validation covered:

- Spring Boot starter jar verification
- Quarkus extension jar verification
- Maven POM and module publication into the static repository layout
- adapter jar manifest `Coakka-Runtime-Jvm-Dependency` parity with the current
  runtime JVM Maven release
- public artifact manifest and surface verification
