# npm Runtime Candidate 37816322

This directory stages the npm-ready Node.js, Bun, and Electron runtime
packages for native generation `1.4.1+9e02a51d`.

Status: staged and verified, not published.

Every tarball contains exactly Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64 native libraries. The copies here are byte-identical to the
artifacts in the corresponding runtime release directories.

Candidate coordinates:

- `coakka-v2-connector-node@1.4.6`
- `coakka-v2-connector-bun@1.4.6`
- `coakka-v2-connector-electron@1.4.6`

Publication order is Node.js, Bun, then Electron. The candidate remains staged
until npm serves each exact version and shasum under the `latest` dist-tag.
