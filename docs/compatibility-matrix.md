# Public Compatibility Matrix

This page is the quick compatibility view for the current public artifact
surface. The root README remains the canonical current-status summary, and
per-drop details remain in [releases/](releases/).

## Runtime 1.3.2 Train

All runtime connector lanes below align to native runtime generation
`1.3.2+caff6d6d` unless a later release note explicitly says otherwise.

| Surface | Public artifact or coordinate | Public generation | Native generation |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.2+caff6d6d/coakka-runtime-native-v2-1.3.2.tar.gz` | `1.3.2+caff6d6d` | `1.3.2+caff6d6d` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.3.2-gcaff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.3.2-gcaff6d6d-6d5ea58` | via JVM runtime |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.3.2-gcaff6d6d-6d5ea58` | via JVM runtime |
| Python runtime | `coakka_v2_connector` wheel | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Node.js runtime | `coakka-v2-connector-node` tarball | `1.3.2+caff6d6d-b46f705` package `1.3.9` | `1.3.2+caff6d6d` |
| Bun runtime | `coakka-v2-connector-bun` tarball | `1.3.2+caff6d6d-b46f705` package `1.3.9` | `1.3.2+caff6d6d` |
| Electron runtime | `coakka-v2-connector-electron` tarball | `1.3.2+caff6d6d-b46f705` package `1.3.9` | via matching Node tarball |
| Tauri runtime | `coakka-runtime-tauri-intents` source package | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Go runtime | `coakka-v2-connector-go` source package | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| C# runtime | `CoAkka.Runtime` package | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Rust runtime | `coakka-runtime-rs` package | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Mojo runtime | `coakka-runtime-mojo` source package | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Zig runtime | `coakka-runtime-zig` source package | `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |

## Logger 1.2.1 Train

All logger connector lanes below align to logger native generation
`1.2.1+f50756ebff0d` unless a later release note explicitly says otherwise.

| Surface | Public artifact or coordinate | Public generation | Logger native generation |
| --- | --- | --- | --- |
| Native C/C++ logger | `logger/native/releases/1.2.1+f50756ebff0d/coakka-logger-native-1.2.1.tar.gz` | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| JVM logger | `coakka.logger:coakka-jvm-native-logger` | `1.2.1-gf50756ebff0d` | `1.2.1+f50756ebff0d` |
| Python logger | `coakka_logger` package | PyPI `1.2.2` | `1.2.1+f50756ebff0d` |
| Node.js logger | `coakka-logger-node` package | npm `1.2.6` from connector `b052a3f` | `1.2.1+f50756ebff0d` |
| Bun logger | `coakka-logger-bun` package | npm `1.2.6` from connector `b052a3f` | `1.2.1+f50756ebff0d` |
| Electron logger | `coakka-logger-electron` package | npm `1.2.6` from connector `b052a3f` | via `coakka-logger-node@1.2.6` |
| Tauri logger | `coakka-logger-tauri-intents` source package | `1.2.1+f50756ebff0d-3e8a6ae` | `1.2.1+f50756ebff0d` |
| Go logger | `coakka-logger-go` source package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| C# logger | `CoAkka.Logger` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Rust logger | `coakka-logger-rs` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Mojo logger | `coakka-logger-mojo` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |
| Zig logger | `coakka-logger-zig` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |

## Runtime Tools

| Surface | Public generation | Public location |
| --- | --- | --- |
| `coakka-client` native CLI | `1.3.2+caff6d6d` | `cli/releases/` |
| `coakka-client` Docker Linux bundle | `1.3.2+caff6d6d` | `demo/coakka-client/releases/` |
| `coakka-runtime-inspect` native UI | `1.3.2+caff6d6d` | `runtime-inspect/native/releases/` |

## Package-Manager Channels

| Channel | Intended first surface | Status |
| --- | --- | --- |
| npm | Node.js, Bun, and Electron runtime/logger packages | current; registry verified |
| PyPI | Python runtime/logger wheels | runtime `coakka-v2-connector==1.3.4` and logger `coakka-logger==1.2.2` current |
| Go modules | Go runtime/logger packages | planned |
| crates.io | Rust runtime/logger packages and Tauri host-side helpers | planned |
| apt/deb | `coakka-client`, `coakka-runtime-inspect`, and possible native dev packages | planned |

Runtime npm packages are published for the sealed native bridge:

| Surface | npm coordinate | Native generation |
| --- | --- | --- |
| Node.js runtime | `coakka-v2-connector-node@1.3.9` | `1.3.2+caff6d6d` |
| Bun runtime | `coakka-v2-connector-bun@1.3.9` | `1.3.2+caff6d6d` |
| Electron runtime | `coakka-v2-connector-electron@1.3.9` | via `coakka-v2-connector-node@1.3.9` |
| Node.js logger | `coakka-logger-node@1.2.6` | `1.2.1+f50756ebff0d` |
| Bun logger | `coakka-logger-bun@1.2.6` | `1.2.1+f50756ebff0d` |
| Electron logger | `coakka-logger-electron@1.2.6` | via `coakka-logger-node@1.2.6` |

Runtime PyPI package:

| Surface | PyPI coordinate | Native generation |
| --- | --- | --- |
| Python runtime | `coakka-v2-connector==1.3.4` | `1.3.2+caff6d6d` |

Logger PyPI package:

| Surface | PyPI coordinate | Logger native generation |
| --- | --- | --- |
| Python logger | `coakka-logger==1.2.2` | `1.2.1+f50756ebff0d` |

See [package-manager-roadmap.md](package-manager-roadmap.md) for remaining
planned package-manager channels.

## Compatibility Rule

Treat the native runtime or logger native generation as the compatibility
anchor. Do not mix language or framework artifacts from another generation
unless a release note explicitly declares that combination compatible.

Consumer-facing downloads are listed in
[`artifacts/public-artifacts.tsv`](../artifacts/public-artifacts.tsv) and are
verified by checksum before public sample use.
