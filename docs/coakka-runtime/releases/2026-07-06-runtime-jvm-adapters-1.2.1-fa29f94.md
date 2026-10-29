# 2026-07-06 Runtime JVM Adapters 1.2.1 Refresh fa29f94

This note records the Spring Boot and Quarkus adapter refresh over JVM runtime
`1.2.1-gabde383-fa29f94`.

| Surface | Public artifact | Version | Runtime JVM dependency |
| --- | --- | --- | --- |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.2.1-gfa29f94b59f9` | `1.2.1-gabde383-fa29f94` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.2.1-gfa29f94b59f9` | `1.2.1-gabde383-fa29f94` |

The adapter source line now follows connector source commit `fa29f94` while
rebasing the published Spring Boot and Quarkus Maven coordinates onto the
current `1.2.1` runtime line. This cut closes the publish verifier dependency
chain for the full runtime family rather than only the JVM lane in isolation.

Validation covered:

- Spring Boot starter jar verification
- Quarkus extension jar verification
- Maven POM and module publication into the static repository layout
- adapter jar manifest `Coakka-Runtime-Jvm-Dependency` parity with the current
  runtime JVM Maven release
- public artifact manifest and surface verification
