# Public Compatibility Matrix

This page is the quick compatibility view for the current public artifact
surface. The root README remains the canonical current-status summary, and
per-drop details remain in [releases/](releases/).

## Runtime 1.3.1 Train

All runtime connector lanes below align to native runtime generation
`1.3.1+bda2ef5` unless a later release note explicitly says otherwise.

| Surface | Public artifact or coordinate | Public generation | Native generation |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.1+bda2ef5/coakka-runtime-native-v2-1.3.1.tar.gz` | `1.3.1+bda2ef5` | `1.3.1+bda2ef5` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.3.1-gbda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.3.1-g0a0aa76` | via JVM runtime |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.3.1-g0a0aa76` | via JVM runtime |
| Python runtime | `coakka_v2_connector` wheel | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Node.js runtime | `coakka-v2-connector-node` package | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Bun runtime | `coakka-v2-connector-bun` package | `1.3.1+bda2ef5-247df1b` | `1.3.1+bda2ef5` |
| Electron runtime | `coakka-v2-connector-electron` package | `1.3.1+bda2ef5-4e0cab0` | `1.3.1+bda2ef5` |
| Tauri runtime | `coakka-runtime-tauri-intents` source package | `1.3.1+bda2ef5-247df1b` | `1.3.1+bda2ef5` |
| Go runtime | `coakka-v2-connector-go` source package | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| C# runtime | `CoAkka.Runtime` package | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Rust runtime | `coakka-runtime-rs` package | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Mojo runtime | `coakka-runtime-mojo` source package | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Zig runtime | `coakka-runtime-zig` source package | `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |

## Logger 1.2.1 Train

All logger connector lanes below align to logger native generation
`1.2.1+f50756ebff0d` unless a later release note explicitly says otherwise.

| Surface | Public artifact or coordinate | Public generation | Logger native generation |
| --- | --- | --- | --- |
| Native C/C++ logger | `logger/native/releases/1.2.1+f50756ebff0d/coakka-logger-native-1.2.1.tar.gz` | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| JVM logger | `coakka.logger:coakka-jvm-native-logger` | `1.2.1-gf50756ebff0d` | `1.2.1+f50756ebff0d` |
| Python logger | `coakka_logger` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Node.js logger | `coakka-logger-node` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Bun logger | `coakka-logger-bun` package | `1.2.1+f50756ebff0d-6fdcc69` | `1.2.1+f50756ebff0d` |
| Electron logger | `coakka-logger-electron` package | `1.2.1+f50756ebff0d-3e8a6ae` | `1.2.1+f50756ebff0d` |
| Tauri logger | `coakka-logger-tauri-intents` source package | `1.2.1+f50756ebff0d-3e8a6ae` | `1.2.1+f50756ebff0d` |
| Go logger | `coakka-logger-go` source package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| C# logger | `CoAkka.Logger` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Rust logger | `coakka-logger-rs` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Mojo logger | `coakka-logger-mojo` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |
| Zig logger | `coakka-logger-zig` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |

## Runtime Tools

| Surface | Public generation | Public location |
| --- | --- | --- |
| `coakka-client` native CLI | `1.3.1+2215b0f` | `cli/releases/` |
| `coakka-client` Docker Linux bundle | `1.3.1+2215b0f` | `demo/coakka-client/releases/` |
| `coakka-runtime-inspect` Linux x86_64/ARM64 | `1.3.1+4ce41f19` | `runtime-inspect/native/releases/` |
| `coakka-runtime-inspect` macOS ARM64 and Windows x86_64/ARM64 | `1.3.1+d7ab7fa` | `runtime-inspect/native/releases/` |

## Package-Manager Channels

| Channel | Intended first surface | Status |
| --- | --- | --- |
| npm | Node.js, Bun, and Electron runtime/logger packages | current; registry verified |
| Go modules | Go runtime/logger packages | planned |
| crates.io | Rust runtime/logger packages and Tauri host-side helpers | planned |
| apt/deb | `coakka-client`, `coakka-runtime-inspect`, and possible native dev packages | planned |

Current npm coordinates:

| Surface | npm coordinate | Native generation |
| --- | --- | --- |
| Node.js runtime | `coakka-v2-connector-node@1.3.2` | `1.3.1+bda2ef5` |
| Bun runtime | `coakka-v2-connector-bun@1.3.2` | `1.3.1+bda2ef5` |
| Electron runtime | `coakka-v2-connector-electron@1.3.2` | via `coakka-v2-connector-node@1.3.2` |
| Node.js logger | `coakka-logger-node@1.2.2` | `1.2.1+f50756ebff0d` |
| Bun logger | `coakka-logger-bun@1.2.2` | `1.2.1+f50756ebff0d` |
| Electron logger | `coakka-logger-electron@1.2.2` | via `coakka-logger-node@1.2.2` |

See [package-manager-roadmap.md](package-manager-roadmap.md) for remaining
planned package-manager channels.

## Compatibility Rule

Treat the native runtime or logger native generation as the compatibility
anchor. Do not mix language or framework artifacts from another generation
unless a release note explicitly declares that combination compatible.

Consumer-facing downloads are listed in
[`artifacts/public-artifacts.tsv`](../artifacts/public-artifacts.tsv) and are
verified by checksum before public sample use.
