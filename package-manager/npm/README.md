# npm Connector Packages

The npm packages are JavaScript connectors in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a JavaScript-only
runtime: Node.js, Bun, and Electron use the same native core and public runtime
contract as the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

| Host | Current runtime package |
| --- | --- |
| Node.js | `coakka-v2-connector-node@1.4.6` |
| Bun | `coakka-v2-connector-bun@1.4.6` |
| Electron | `coakka-v2-connector-electron@1.4.6` |

These three published coordinates use native generation `1.4.1+9e02a51d` and
connector source `37816322`. Their immutable pre-publication artifacts and
checksums remain recorded under `candidates/37816322/`.

The Node.js registry package executes request/reply on macOS ARM64 and Linux
ARM64/x86-64. See the exact [package and platform evidence](../../docs/runtime-package-platform-evidence.md)
before selecting another host target.

Kubernetes is supported but not required. Select an exact package version and
native OS/CPU tuple using the [current packages](../../docs/current-packages.md)
and [compatibility matrix](../../docs/compatibility-matrix.md). Common loader,
checksum, signing, and certificate failures are covered in
[troubleshooting](../../docs/troubleshooting.md).
