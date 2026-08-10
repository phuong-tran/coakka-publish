# npm Connector Packages

The npm packages are JavaScript connectors in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a JavaScript-only
runtime: Node.js, Bun, and Electron use the same native core and public runtime
contract as the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

| Host | Current runtime package |
| --- | --- |
| Node.js | `coakka-v2-connector-node@2.3.0` |
| Bun | `coakka-v2-connector-bun@2.3.0` |
| Electron | `coakka-v2-connector-electron@2.3.0` |

These published coordinates use native generation `2.3.0+a83ab412` and
connector source `3a84c7b`. Their immutable publication receipt, registry
integrity values, artifacts, and checksums are under `candidates/3a84c7b/`.

`2.3.0` adds the official Stream Lane connector surface while retaining File
Lane, request/reply, deadletter, and lifecycle behavior. The runtime reports
transport pressure and terminal state; adaptation policy remains in the
app-host.

## Runtime 2.3.0 Artifacts

The public artifact mirror contains Node.js, Bun, and Electron `2.3.0` packages
from connector source `3a84c7b` over native generation `2.3.0+a83ab412`.
Each package is checksum-pinned in `artifacts/public-artifacts.tsv`; Node.js and
Bun include Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64 native
payloads. Packaged consumer smokes load runtime `2.3.0` and complete real
request/reply. npm registry downloads match the staged tarballs byte-for-byte;
clean Node.js and Bun installs complete request/reply, and Electron resolves
its exact Node.js `2.3.0` dependency.

The Node.js registry package executes request/reply on macOS ARM64 and Linux
ARM64/x86-64. See the exact [package and platform evidence](../../docs/runtime-package-platform-evidence.md)
before selecting another host target.

Kubernetes is supported but not required. Select an exact package version and
native OS/CPU tuple using the [current packages](../../docs/current-packages.md)
and [compatibility matrix](../../docs/compatibility-matrix.md). Common loader,
checksum, signing, and certificate failures are covered in
[troubleshooting](../../docs/troubleshooting.md).
