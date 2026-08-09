# npm Runtime Patch Release 80cdd77

Status: published to npm and registry-verified on 2026-08-09.

This receipt records the Node.js, Bun, and Electron `2.1.1` documentation
patch over native generation `2.1.0+60ddf70d`:

- `coakka-v2-connector-node@2.1.1`
- `coakka-v2-connector-bun@2.1.1`
- `coakka-v2-connector-electron@2.1.1`

The patch replaces a README link to the non-public connector source repository
with the canonical public file-lane contract and removes stale wording that
implied the bundled runtime was not sendfile-capable. Node.js and Bun still
contain exactly five runtime libraries: Linux ARM64/x86-64, macOS ARM64, and
Windows ARM64/x86-64. Electron depends on exact Node.js `2.1.1`.

Post-publication verification matched every registry shasum to the staged
tarball. Clean packaged smokes completed Node.js and Bun native request/reply
and the Electron main-process intent path. The packaged README regression gate
requires the canonical public contract URL and rejects connector-source URLs.
