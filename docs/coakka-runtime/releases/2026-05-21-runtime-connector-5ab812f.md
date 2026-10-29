# 2026-05-21 Runtime Connector UX Refresh

This note records the CoAkka runtime connector UX refresh for source connector
commit `5ab812f` over native package generation `0.2.0+94a5729`.

| Surface | Public artifact | Version |
| --- | --- | --- |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `0.2.0-g94a5729-5ab812f` |
| Python runtime | `coakka_v2_connector` wheel | `0.2.0` |
| Node.js runtime | `coakka-v2-connector-node` package | `0.2.0` |
| Go runtime | `coakka-v2-connector-go` source package | `0.2.0` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `0.2.0-g5ab812f` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `0.2.0-g5ab812f` |

The refresh keeps the same native package generation as the previous JVM,
Python, Node.js, and Go runtime connector set. C# and Rust remain on
`0.2.0+94a5729-6b7a3bf`.

User-facing changes:

- Kotlin local first-run helper: `CoAkka.local(...)`
- typed route and endpoint flag wrappers for JVM connectors
- text payload helper aliases for JVM, Python, Node.js, and Go
- local route helper aliases for Python, Node.js, and Go
- Spring Boot and Quarkus adapters rebuilt against the refreshed JVM runtime

Validation covered:

- JVM packaged-jar smoke
- Python packaged-wheel smoke
- Node.js packaged-tarball smoke
- Go packaged-source smoke
- Spring Boot and Quarkus Maven publication against the refreshed JVM runtime
- runtime intake gate for JVM, Python, Node.js, and Go artifacts
