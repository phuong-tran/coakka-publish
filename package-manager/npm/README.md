# npm Connector Packages

The npm packages are JavaScript connectors in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a JavaScript-only
runtime: Node.js, Bun, and Electron use the same native core and public runtime
contract as the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

| Host | Current runtime package |
| --- | --- |
| Node.js | `coakka-v2-connector-node@2.5.2` |
| Bun | `coakka-v2-connector-bun@2.5.2` |
| Electron | `coakka-v2-connector-electron@2.5.2` |

These published coordinates use native generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a` and connector candidate
`3ae74f4`. Their immutable publication receipt, registry integrity values,
artifacts, and checksums are under `candidates/3ae74f4/`.

`2.5.0` adds the current File Lane and Stream Lane connector surfaces while
retaining the macOS 13, Node.js 22, Bun 1.2.0, and Electron 42 host floors.
`2.5.2` adds typed replica-owner grants, trusted control-plane reconstruction,
and exact-owner lane pinning to those surfaces.

## Runtime 2.5.2 Packages

The public artifact mirror contains Node.js, Bun, and Electron `2.5.2` packages
from connector candidate `3ae74f4` over native generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.
Each package is checksum-pinned in `artifacts/public-artifacts.tsv`. Bundled
native payloads cover Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64.
Clean registry smokes load runtime `2.5.0` and complete real request/reply. npm
registry downloads match the sealed candidate tarballs byte-for-byte;
clean Node.js and Bun installs complete request/reply, and Electron resolves
its exact Node.js `2.5.2` dependency and completes the main-process intent path.

The Node.js registry package executes request/reply on macOS ARM64 and Linux
ARM64/x86-64. See the exact [package and platform evidence](../../docs/runtime-package-platform-evidence.md)
before selecting another host target.

Kubernetes is supported but not required. Select an exact package version and
native OS/CPU tuple using the [current packages](../../docs/current-packages.md)
and [compatibility matrix](../../docs/compatibility-matrix.md). Common loader,
checksum, signing, and certificate failures are covered in
[troubleshooting](../../docs/troubleshooting.md).
