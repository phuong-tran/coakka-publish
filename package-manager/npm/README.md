# npm Connector Packages

The npm packages are JavaScript connectors in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a JavaScript-only
runtime: Node.js, Bun, and Electron use the same native core and public runtime
contract as the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

| Host | Current runtime package |
| --- | --- |
| Node.js | `coakka-v2-connector-node@2.1.0` |
| Bun | `coakka-v2-connector-bun@2.1.0` |
| Electron | `coakka-v2-connector-electron@2.1.0` |

These published coordinates use native generation `2.1.0+60ddf70d` and
connector source `4782dcd`. Their immutable publication receipt, registry
integrity values, artifacts, and checksums are under `candidates/4782dcd/`.

## Runtime 2.1.0 Artifacts

The public artifact mirror contains Node.js, Bun, and Electron `2.1.0` packages
from connector source `4782dcd` over native generation `2.1.0+60ddf70d`.
Each package is checksum-pinned in `artifacts/public-artifacts.tsv`; Node.js and
Bun include Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64 native
payloads. Packaged consumer smokes load runtime `2.1.0` and complete real
request/reply. npm registry downloads match the staged tarballs byte-for-byte;
clean Node.js and Bun installs complete request/reply, and Electron resolves
its exact Node.js `2.1.0` dependency.

The Node.js registry package executes request/reply on macOS ARM64 and Linux
ARM64/x86-64. See the exact [package and platform evidence](../../docs/runtime-package-platform-evidence.md)
before selecting another host target.

Kubernetes is supported but not required. Select an exact package version and
native OS/CPU tuple using the [current packages](../../docs/current-packages.md)
and [compatibility matrix](../../docs/compatibility-matrix.md). Common loader,
checksum, signing, and certificate failures are covered in
[troubleshooting](../../docs/troubleshooting.md).
