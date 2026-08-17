# Public Compatibility Matrix

This page is the quick compatibility view for the current public artifact
surface. The root README remains the canonical current-status summary, and
per-drop details remain in [releases/](releases/).

## Ecosystem Scope

CoAkka is not Kubernetes-only. Kubernetes receives detailed operational
coverage because topology, rollout, policy, and scale are substantial, while
the runtime contract remains deployment-neutral.

| Dimension | Current public scope | Verification boundary |
| --- | --- | --- |
| Languages and frameworks | Native C/C++, JVM, Spring Boot, Quarkus, Node.js, Bun, Electron, Python, Go, C#, Rust, Swift, Zig, Mojo, and Tauri lanes | A named lane is not automatically available on every OS; use the exact artifact and generation rows below. |
| Native platforms | Linux x86-64 and ARM64, macOS ARM64, and Windows x86-64 and ARM64 appear in the current runtime, tool, or evidence artifact ledger | Each artifact may carry a smaller platform subset; verify its manifest and checksum. |
| Deployment shapes | Standalone hosts, containers, Kubernetes, VMs, bare metal, and architecture-matched edge deployments | Device and industrial Android support requires release-specific ABI, dependency, clock, certificate, lifecycle, and execution evidence. |

Across those surfaces, polyglot connectors preserve one target,
request/reply, bounded-admission, deadletter, capability, and configuration
contract instead of inventing language-specific runtime semantics.

## Runtime 2.4.0 Artifact Train

The current artifact mirror aligns every runtime connector lane and the JVM
framework adapters to native generation `2.4.0+c2f53117`. Runtime tools retain
their separately listed 2.3.0 generation. The runtime train contains five libraries: Linux
ARM64/x86-64 `.so`, macOS ARM64 `.dylib`, and Windows ARM64/x86-64 `.dll`.

| Surface | Public artifact or coordinate | Public generation | Native generation |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/2.4.0+c2f53117/coakka-runtime-native-v2-2.4.0.tar.gz` | `2.4.0+c2f53117` | `2.4.0+c2f53117` |
| JVM runtime | `io.github.phuong-tran.coakka:runtime` | `2.4.1` | `2.4.0+c2f53117` |
| Spring Boot adapter | `io.github.phuong-tran.coakka:spring-boot-starter` | `2.4.1` | via JVM runtime |
| Quarkus adapter | `io.github.phuong-tran.coakka:quarkus-extension` | `2.4.1` | via JVM runtime |
| Language connector archives | `runtime/<lane>/releases/2.4.0+c2f53117-0afb5e9/` | `2.4.0` | `2.4.0+c2f53117` |
| npm | `coakka-v2-connector-{node,bun,electron}@2.4.0` | `2.4.0` | `2.4.0+c2f53117` |
| PyPI | `coakka-v2-connector==2.4.0` | `2.4.0` | `2.4.0+c2f53117` |
| NuGet | `CoAkka.Runtime==2.4.1` | `2.4.1` | `2.4.0+c2f53117` |
| Go module | `github.com/phuong-tran/coakka-runtime-go` | `v1.7.1` | `2.4.0+c2f53117` |
| SwiftPM | `github.com/phuong-tran/coakka-runtime-swift` | `v2.4.1` | `2.4.0+c2f53117` |

The artifact mirror is complete independently of npm, PyPI, and NuGet. Exact
registry coordinates and authenticated publication receipts are listed below.

## Runtime 1.4.1 Train

The public artifact mirror aligns the runtime connector lanes below to native
runtime generation `1.4.1+9e02a51d`. Package-manager registries have independent
publication state and remain listed separately below.

Package presence, verification, execution, and known failures are stated
separately in [Runtime Package And Platform Evidence](runtime-package-platform-evidence.md).

| Surface | Public artifact or coordinate | Public generation | Native generation |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.4.1+9e02a51d/coakka-runtime-native-v2-1.4.1.tar.gz` | `1.4.1+9e02a51d` | `1.4.1+9e02a51d` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.4.1-g9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.4.1-g9e02a51d-4e7cda4` | via JVM runtime |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.4.1-g9e02a51d-4e7cda4` | via JVM runtime |
| Python runtime artifact | `coakka_v2_connector` wheel | PyPI `1.4.6`, source `603a3728`, mirrored in `1.4.1+9e02a51d-603a3728` | `1.4.1+9e02a51d` |
| Node.js runtime artifact | `coakka-v2-connector-node` tarball | npm `1.4.6`, source `37816322`, mirrored in `1.4.1+9e02a51d-37816322` | `1.4.1+9e02a51d` |
| Bun runtime artifact | `coakka-v2-connector-bun` tarball | npm `1.4.6`, source `37816322`, mirrored in `1.4.1+9e02a51d-37816322` | `1.4.1+9e02a51d` |
| Electron runtime artifact | `coakka-v2-connector-electron` tarball | npm `1.4.6`, source `37816322`, mirrored in `1.4.1+9e02a51d-37816322` | via matching Node artifact |
| Tauri runtime artifact | `coakka-runtime-tauri-intents` source package | `1.4.1-source` in `1.4.1+9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |
| Go runtime artifact | `coakka-v2-connector-go` module archive | `1.4.1` in `1.4.1+9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |
| C# runtime artifact | `CoAkka.Runtime` package file | NuGet `1.4.7` from `1.4.1+9e02a51d-af244b1e` | `1.4.1+9e02a51d` |
| Rust runtime artifact | `coakka-runtime-rs` source package | `1.4.1` in `1.4.1+9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |
| Swift runtime artifact | `coakka-runtime-swift` source package | `1.4.1` in `1.4.1+9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |
| Mojo runtime artifact | `coakka-runtime-mojo` source package | `1.4.1-source` in `1.4.1+9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |
| Zig runtime artifact | `coakka-runtime-zig` source package | `1.4.1-source` in `1.4.1+9e02a51d-4e7cda4` | `1.4.1+9e02a51d` |

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
| Go logger | `github.com/phuong-tran/coakka-logger-go` Go module | `v1.2.6` | `1.2.1+f50756ebff0d` |
| C# logger | `CoAkka.Logger` NuGet package | `1.2.3` | `1.2.1+f50756ebff0d` |
| Rust logger | `coakka-logger-rs` package | `1.2.1+f50756ebff0d` | `1.2.1+f50756ebff0d` |
| Swift logger | `github.com/phuong-tran/coakka-logger-swift` SwiftPM package | `v1.2.2` | `1.2.1+f50756ebff0d` |
| Mojo logger | `coakka-logger-mojo` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |
| Zig logger | `coakka-logger-zig` source package | `1.2.1+f50756ebff0d-8264bba` | `1.2.1+f50756ebff0d` |

## Runtime Tools

| Surface | Public generation | Public location |
| --- | --- | --- |
| `coakka-client` native CLI | all five native platforms `2.4.0+c2f53117` | `coakka-tools/coakka-client/releases/2.4.0+c2f53117/` |
| `coakka-client` Docker Linux bundle | `1.3.2+caff6d6d` | `coakka-tools/coakka-client/docker-demo/releases/` |
| `coakka-runtime-inspect` native UI | all five native platforms `2.4.0+c2f53117` | `coakka-tools/coakka-runtime-inspect/releases/2.4.0+c2f53117/` |

## Package-Manager Channels

| Channel | Available surface | Status |
| --- | --- | --- |
| npm | Node.js, Bun, and Electron runtime/logger packages | runtime `2.4.0`, logger `1.2.6`; registry verified |
| PyPI | Python runtime/logger wheels | runtime `coakka-v2-connector==2.4.0` and logger `coakka-logger==1.2.2`; registry verified |
| Go modules | Go runtime/logger packages | runtime `github.com/phuong-tran/coakka-runtime-go@v1.7.1`, logger `github.com/phuong-tran/coakka-logger-go@v1.2.6` |
| SwiftPM | Swift runtime/logger source packages | runtime `github.com/phuong-tran/coakka-runtime-swift@2.4.1`, logger `github.com/phuong-tran/coakka-logger-swift@1.2.2` |
| NuGet | C# runtime/logger packages | runtime `CoAkka.Runtime==2.4.1`, logger `CoAkka.Logger==1.2.3`; `net8.0` minimum, registry verified on .NET 8/9/10 |

PyPI superseded versions are yanked, not deleted:
`coakka-v2-connector==1.3.2`, `coakka-v2-connector==1.3.3`, and
`coakka-logger==1.2.1`. Use the current PyPI versions listed above.

Runtime npm packages are published for the sealed package boundary:

| Surface | npm coordinate | Native generation |
| --- | --- | --- |
| Node.js runtime | `coakka-v2-connector-node@2.4.0` | `2.4.0+c2f53117` |
| Bun runtime | `coakka-v2-connector-bun@2.4.0` | `2.4.0+c2f53117` |
| Electron runtime | `coakka-v2-connector-electron@2.4.0` | via `coakka-v2-connector-node@2.4.0` |
| Node.js logger | `coakka-logger-node@1.2.6` | `1.2.1+f50756ebff0d` |
| Bun logger | `coakka-logger-bun@1.2.6` | `1.2.1+f50756ebff0d` |
| Electron logger | `coakka-logger-electron@1.2.6` | via `coakka-logger-node@1.2.6` |

Runtime PyPI package:

| Surface | PyPI coordinate | Native generation |
| --- | --- | --- |
| Python runtime | `coakka-v2-connector==2.4.0` | `2.4.0+c2f53117` |

Logger PyPI package:

| Surface | PyPI coordinate | Logger native generation |
| --- | --- | --- |
| Python logger | `coakka-logger==1.2.2` | `1.2.1+f50756ebff0d` |

NuGet packages:

| Surface | NuGet coordinate | Native generation |
| --- | --- | --- |
| C# runtime | `CoAkka.Runtime==2.4.1` (`net8.0`; verified on .NET 8/9/10) | `2.4.0+c2f53117` |
| C# logger | `CoAkka.Logger==1.2.3` (`net8.0`; verified on .NET 8/9/10) | `1.2.1+f50756ebff0d` |

SwiftPM packages:

| Surface | SwiftPM coordinate | Native generation |
| --- | --- | --- |
| Swift runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `2.4.1` | `2.4.0+c2f53117` |
| Swift logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.2` | `1.2.1+f50756ebff0d` |

See [package-manager-roadmap.md](package-manager-roadmap.md) for current
package-manager coordinates.

## Compatibility Rule

Treat the native runtime or logger native generation as the compatibility
anchor. Do not mix language or framework artifacts from another generation
unless a release note explicitly declares that combination compatible.

Consumer-facing downloads are listed in
[`artifacts/public-artifacts.tsv`](../artifacts/public-artifacts.tsv) and are
verified by checksum before public sample use.
