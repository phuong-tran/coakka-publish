# JavaScript And Desktop Host Compatibility

Status: published and registry-verified on August 18, 2026.

The Runtime and Logger Node.js, Bun, Electron, and Tauri lanes previously
bundled macOS ARM64 dylibs built with an unintended macOS 26.0 deployment
target. This patch rebuilds only those dylibs from the same native source
identities with deployment target 13.0 and leaves their C ABIs and native
generations unchanged.

## Coordinates

| Lane | Runtime | Logger | Minimum host |
| --- | --- | --- | --- |
| Node.js | `2.4.1` | `1.2.7` | Node.js 22 |
| Bun | `2.4.1` | `1.2.7` | Bun 1.2.0 |
| Electron | `2.4.1` | `1.2.7` | Electron 42; Node.js 22 main process |
| Tauri source | `2.4.1-source` | `1.2.2-source` | Rust 1.77.2; Tauri 2 application shape |

Runtime packages retain native generation `2.4.0+c2f53117`; logger packages
retain `1.2.1+f50756ebff0d`. The Tauri crates expose a trusted Rust intent
boundary and do not depend directly on Tauri, so the consuming app remains
responsible for any higher MSRV introduced by its own dependency graph.

## Corrected Payloads

```text
956f6cf04c18a923cc6416366b1a1ee1e5cae67e6f61bf3988e6bbeb09db6a7c  libcoakka_runtime_v2.dylib
28db676f3a56ea67c909697d82453edd76372bd8c9916dd43250f453cfedc87e  libcoakka_logger_core.dylib
```

Both identify the same reviewed native source snapshots as the existing
generations and declare macOS deployment target 13.0. Package staging and the
public candidate verifier now parse Mach-O load commands and fail closed on a
missing or different target in addition to checking the SHA-256 digest.

## Evidence

- Node.js 22.23.2 passes runtime File Lane, Stream Lane, network-policy tests,
  packaged native request/reply, and logger emit/drain on macOS ARM64.
- Bun 1.2.0 passes native request/reply, File Lane, Stream Lane, and logger
  emit/drain on macOS ARM64.
- Electron 42 passes clean packaged runtime and logger renderer/preload/main
  intent smokes on macOS ARM64.
- Rust 1.77.2 compiles the Runtime, Logger, and both Tauri intent crates.
  Extracted Tauri archives execute runtime request/reply and logger emit/drain.
- npm candidate `package-manager/npm/candidates/7718ce6/` passes checksums,
  public metadata, package boundary, exact dependency, engine/peer range, and
  deployment-target gates for all six packages.
- Registry metadata, SHA-1, integrity, and downloaded tarball bytes match the
  candidate for all six npm packages. Clean Node.js and Bun consumers complete
  Runtime request/reply and Logger emit/drain; Electron 42 consumers complete
  the Runtime and Logger main/preload intent paths.
- Runtime Tauri `2.4.1-source` passes command and desktop application consumers;
  Logger Tauri `1.2.2-source` passes its Rust-owned log-intent consumer.

The matching-host executions above ran on a newer macOS ARM64 host and do not
claim execution on macOS 13. Connector CI run `32058042610` passes at source
commit `f36c396`, covering Node.js 22/24, Bun 1.2/current, Electron 42/43,
Rust 1.77.2/stable, Spring Boot, Quarkus, platform-source compilation, and the
aggregate JVM, Python, Node.js, and NuGet gates.
