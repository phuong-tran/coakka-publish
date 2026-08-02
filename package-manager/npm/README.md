# npm Connector Packages

The npm packages are JavaScript connectors in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a JavaScript-only
runtime: Node.js, Bun, and Electron use the same native core and public runtime
contract as the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

| Host | Current runtime package |
| --- | --- |
| Node.js | `coakka-v2-connector-node@1.4.5` |
| Bun | `coakka-v2-connector-bun@1.4.5` |
| Electron | `coakka-v2-connector-electron@1.4.5` |

These three coordinates use native generation `1.4.0+2cee86bf` and connector
source `2c0c598`. The exact registry candidates and checksums are recorded
under `candidates/2c0c598/`.

Kubernetes is supported but not required. Select an exact package version and
native OS/CPU tuple using the [current packages](../../docs/current-packages.md)
and [compatibility matrix](../../docs/compatibility-matrix.md). Common loader,
checksum, signing, and certificate failures are covered in
[troubleshooting](../../docs/troubleshooting.md).
