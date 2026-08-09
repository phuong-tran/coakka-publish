# npm Runtime Release 4782dcd

Status: published to npm and registry-verified on 2026-08-09.

This receipt records the Node.js, Bun, and Electron `2.1.0` packages over
native generation `2.1.0+60ddf70d`:

- `coakka-v2-connector-node@2.1.0`
- `coakka-v2-connector-bun@2.1.0`
- `coakka-v2-connector-electron@2.1.0`

Node.js and Bun contain exactly five runtime libraries: Linux ARM64/x86-64,
macOS ARM64, and Windows ARM64/x86-64. Electron depends on the exact Node.js
`2.1.0` package. npm serves all three versions under `latest`.

Post-publication verification downloaded each registry tarball, matched its
SHA-256 byte-for-byte with this candidate, clean-installed Node.js and Bun,
and completed package-surface plus native request/reply smoke. A clean
Electron install resolved both Electron and Node connector version `2.1.0`.
