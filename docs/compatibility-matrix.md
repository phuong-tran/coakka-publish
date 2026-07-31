# Public Compatibility Matrix

This page is the quick compatibility view for the current public artifact
surface. The root README remains the canonical current-status summary, and
per-drop details remain in [releases/](releases/).

## Runtime 1.3.4 Train

The public artifact mirror aligns the runtime connector lanes below to native
runtime generation `1.3.4+dc6ec284`. Package-manager registries have independent
publication state and remain listed separately below.

| Surface | Public artifact or coordinate | Public generation | Native generation |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.4+dc6ec284/coakka-runtime-native-v2-1.3.4.tar.gz` | `1.3.4+dc6ec284` | `1.3.4+dc6ec284` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.3.4-gdc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.3.4-gdc6ec284-f68ff5c` | via JVM runtime |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.3.4-gdc6ec284-f68ff5c` | via JVM runtime |
| Python runtime artifact | `coakka_v2_connector` wheel | `1.3.6` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Node.js runtime artifact | `coakka-v2-connector-node` tarball | `1.3.11` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Bun runtime artifact | `coakka-v2-connector-bun` tarball | `1.3.11` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Electron runtime artifact | `coakka-v2-connector-electron` tarball | `1.3.11` in `1.3.4+dc6ec284-f68ff5c` | via matching Node artifact |
| Tauri runtime artifact | `coakka-runtime-tauri-intents` source package | `1.3.4-source` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Go runtime | `github.com/phuong-tran/coakka-runtime-go` Go module | `v1.3.12` | `1.3.4+dc6ec284` |
| C# runtime artifact | `CoAkka.Runtime` NuGet package file | `1.3.5` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Rust runtime artifact | `coakka-runtime-rs` package | `1.3.4-spike` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Swift runtime | `github.com/phuong-tran/coakka-runtime-swift` SwiftPM package | `v1.3.4` | `1.3.4+dc6ec284` |
| Mojo runtime artifact | `coakka-runtime-mojo` source package | `1.3.4-source` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |
| Zig runtime artifact | `coakka-runtime-zig` source package | `1.3.4-source` in `1.3.4+dc6ec284-f68ff5c` | `1.3.4+dc6ec284` |

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
| Go logger | `github.com/phuong-tran/coakka-logger-go` Go module | `v1.2.5` | `1.2.1+f50756ebff0d` |
| C# logger | `CoAkka.Logger` NuGet package | `1.2.2` | `1.2.1+f50756ebff0d` |
| Rust logger | `coakka-logger-rs` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Swift logger | `github.com/phuong-tran/coakka-logger-swift` SwiftPM package | `v1.2.1` | `1.2.1+f50756ebff0d` |
| Mojo logger | `coakka-logger-mojo` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |
| Zig logger | `coakka-logger-zig` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |

## Runtime Tools

| Surface | Public generation | Public location |
| --- | --- | --- |
| `coakka-client` native CLI | `1.3.4+dc6ec28` | `coakka-tools/coakka-client/releases/` |
| `coakka-client` Docker Linux bundle | `1.3.2+caff6d6d` | `coakka-tools/coakka-client/docker-demo/releases/` |
| `coakka-runtime-inspect` native UI | `1.3.4+dc6ec28` | `coakka-tools/coakka-runtime-inspect/releases/` |

## Package-Manager Channels

| Channel | Intended first surface | Status |
| --- | --- | --- |
| npm | Node.js, Bun, and Electron runtime/logger packages | current; registry verified |
| PyPI | Python runtime/logger wheels | runtime `coakka-v2-connector==1.3.4` and logger `coakka-logger==1.2.2` current |
| Go modules | Go runtime/logger packages | current; runtime `github.com/phuong-tran/coakka-runtime-go@v1.3.12`, logger `github.com/phuong-tran/coakka-logger-go@v1.2.5` |
| SwiftPM | Swift runtime/logger source packages | current; runtime `github.com/phuong-tran/coakka-runtime-swift@1.3.4`, logger `github.com/phuong-tran/coakka-logger-swift@1.2.1` |
| NuGet | C# runtime/logger packages | current; runtime `CoAkka.Runtime==1.3.3`, logger `CoAkka.Logger==1.2.2` |
| crates.io | Rust runtime/logger packages and Tauri host-side helpers | planned |
| apt/deb | `coakka-client`, `coakka-runtime-inspect`, and possible native dev packages | planned |

PyPI superseded versions are yanked, not deleted:
`coakka-v2-connector==1.3.2`, `coakka-v2-connector==1.3.3`, and
`coakka-logger==1.2.1`. Use the current PyPI versions listed above.

Runtime npm packages are published for the sealed package boundary:

| Surface | npm coordinate | Native generation |
| --- | --- | --- |
| Node.js runtime | `coakka-v2-connector-node@1.3.11` | `1.3.4+dc6ec284` |
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

NuGet packages:

| Surface | NuGet coordinate | Native generation |
| --- | --- | --- |
| C# runtime | `CoAkka.Runtime==1.3.3` | `1.3.2+caff6d6d` |
| C# logger | `CoAkka.Logger==1.2.2` | `1.2.1+f50756ebff0d` |

SwiftPM packages:

| Surface | SwiftPM coordinate | Native generation |
| --- | --- | --- |
| Swift runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `1.3.4` | `1.3.4+dc6ec284` |
| Swift logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1` | `1.2.1+f50756ebff0d` |

See [package-manager-roadmap.md](package-manager-roadmap.md) for remaining
planned package-manager channels.

## Compatibility Rule

Treat the native runtime or logger native generation as the compatibility
anchor. Do not mix language or framework artifacts from another generation
unless a release note explicitly declares that combination compatible.

Consumer-facing downloads are listed in
[`artifacts/public-artifacts.tsv`](../artifacts/public-artifacts.tsv) and are
verified by checksum before public sample use.
