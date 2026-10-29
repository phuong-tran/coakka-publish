# 2026-07-05 Runtime JVM Adapter b8ecfae Refresh 1107154

This note records the Spring Boot and Quarkus adapter refresh over JVM runtime
`0.2.0-gb8ecfae-1107154`.

| Surface | Public artifact | Version | Runtime JVM dependency |
| --- | --- | --- | --- |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `0.2.0-g11071541ea78` | `0.2.0-gb8ecfae-1107154` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `0.2.0-g11071541ea78` | `0.2.0-gb8ecfae-1107154` |

The adapter source line now follows the current connector workspace head while
rebasing the published Spring Boot and Quarkus Maven coordinates onto the
`b8ecfae` JVM runtime refresh. This slice does not claim a broader non-JVM
runtime propagation; it closes the publish verifier dependency chain for the
runtime JVM family only.

Validation covered:

- Spring Boot starter jar verification
- Quarkus extension jar verification
- Maven POM and module publication into the static repository layout
- adapter jar manifest `Coakka-Runtime-Jvm-Dependency` parity with the current
  runtime JVM Maven release
- public artifact manifest and surface verification

