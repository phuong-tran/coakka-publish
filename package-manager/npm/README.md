# npm Connector Packages

The npm packages are JavaScript connectors in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a JavaScript-only
runtime: Node.js, Bun, and Electron use the same native core and public runtime
contract as the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

| Host | Current runtime package |
| --- | --- |
| Node.js | `coakka-v2-connector-node@2.5.3` |
| Bun | `coakka-v2-connector-bun@2.5.3` |
| Electron | `coakka-v2-connector-electron@2.5.3` |

These published coordinates use native generation
`2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be` and connector source
`0ba485e8ff19f3ce23902345cb445a1f652fe3f3`. Their immutable artifacts and
checksums are under the matching `runtime/{node,bun,electron}/releases/`
directories.

Runtime `2.5.3` exposes the current File Lane and Stream Lane connector
surfaces, typed replica-owner grants, trusted control-plane reconstruction,
and exact-owner lane pinning while retaining the macOS 13, Node.js 22, Bun
1.2.0, and Electron 42 host floors.

## Runtime 2.5.3 Packages

The public artifact mirror contains Node.js, Bun, and Electron `2.5.3` packages
from connector source `0ba485e` over native generation
`2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`.
Each package is checksum-pinned in `artifacts/public-artifacts.tsv`. Bundled
native payloads cover Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64.
Clean registry smokes load runtime `2.5.0` and complete real request/reply. npm
registry downloads match the sealed candidate tarballs byte-for-byte;
clean Node.js and Bun installs complete request/reply, and Electron resolves
its exact Node.js `2.5.3` dependency and completes the package-surface path.
Exact digests and registry evidence are recorded in the
[Runtime registry packages 2.5.3 receipt](../../docs/releases/2026-08-21-runtime-registries-2.5.3.md).

The Node.js registry package executes request/reply on macOS ARM64 and Linux
ARM64/x86-64. See the exact [package and platform evidence](../../docs/runtime-package-platform-evidence.md)
before selecting another host target.

Kubernetes is supported but not required. Select an exact package version and
native OS/CPU tuple using the [current packages](../../docs/current-packages.md)
and [compatibility matrix](../../docs/compatibility-matrix.md). Common loader,
checksum, signing, and certificate failures are covered in
[troubleshooting](../../docs/troubleshooting.md).
