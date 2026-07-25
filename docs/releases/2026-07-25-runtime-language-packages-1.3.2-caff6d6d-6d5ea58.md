# 2026-07-25 Runtime Language Packages 1.3.2 caff6d6d 6d5ea58

This note records the language package refresh over native runtime core
`1.3.2+caff6d6d`.

| Surface | Artifact | Version |
| --- | --- | --- |
| JVM | `runtime/jvm/releases/1.3.2+caff6d6d-6d5ea58/coakka-jvm-native-runtime-v2-1.3.2-gcaff6d6d-6d5ea58.jar` | `1.3.2-gcaff6d6d-6d5ea58` |
| Python | `runtime/python/releases/1.3.2+caff6d6d-6d5ea58/coakka_v2_connector-1.3.2-py3-none-any.whl` | `1.3.2` |
| Node | `runtime/node/releases/1.3.2+caff6d6d-b46f705/coakka-v2-connector-node-1.3.9.tgz` | `1.3.9` |
| Bun | `runtime/bun/releases/1.3.2+caff6d6d-b46f705/coakka-v2-connector-bun-1.3.9.tgz` | `1.3.9` |
| Electron | `runtime/electron/releases/1.3.2+caff6d6d-b46f705/coakka-v2-connector-electron-1.3.9.tgz` | `1.3.9` |
| Go | `runtime/go/releases/1.3.2+caff6d6d-6d5ea58/coakka-v2-connector-go-1.3.2.tar.gz` | `1.3.2` |
| C# | `runtime/csharp/releases/1.3.2+caff6d6d-6d5ea58/CoAkka.Runtime.1.3.2.nupkg` | `1.3.2` |
| Rust | `runtime/rust/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-rs-1.3.2-spike.tar.gz` | `1.3.2-spike` |
| Mojo | `runtime/mojo/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-mojo-1.3.2-source.tar.gz` | `1.3.2-source` |
| Zig | `runtime/zig/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-zig-1.3.2-source.tar.gz` | `1.3.2-source` |
| Tauri intents | `runtime/tauri/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-tauri-intents-1.3.2-source.tar.gz` | `1.3.2-source` |

Spring Boot and Quarkus Maven adapter artifacts were also refreshed at
`1.3.2-gcaff6d6d-6d5ea58` so their runtime dependency follows the same JVM
runtime artifact generation.

Validation covered:

- connector multi-language gate `scripts/verify_multilang_ports.sh`
- JVM packaged jar smoke and runtime bundle verifier
- Python unit tests, wheel build, and packaged wheel smoke
- Go tests and packaged package smoke
- C# build and packaged NuGet smoke
- Rust tests, runtime smoke, and source package build
- Mojo and Zig source package builds and native runtime smokes
- Tauri intent tests and source package build
- Node, Bun, and Electron package builds and packaged smokes
- public artifact surface verification in `coakka-publish`

The npm registry publish candidate lives under
`package-manager/npm/candidates/b46f705/`. It carries runtime connector
`1.3.9` over native generation `1.3.2+caff6d6d`; the registry moves only after
the publish step completes.
