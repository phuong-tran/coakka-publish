# npm Runtime 2.3.0 Release Receipt

Status: published to npm and registry-verified on 2026-08-10.

This receipt records the Node.js, Bun, and Electron runtime `2.3.0` packages
from connector source `3a84c7b`, payload staging commit `7708105`, and native
generation `2.3.0+a83ab412`:

- `coakka-v2-connector-node@2.3.0`
- `coakka-v2-connector-bun@2.3.0`
- `coakka-v2-connector-electron@2.3.0`

Node.js and Bun contain the exact Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64 native payloads. Electron depends on exact Node.js `2.3.0`. All
three expose the runtime `2.3.0` Stream Lane connector surface.

The registry verifier confirms each published version, `latest` tag, tarball
SHA-1, integrity value, native generation, platform set, and Electron's exact
Node.js dependency against the staged candidate.

Clean registry installs complete Node.js and Bun native request/reply, Node.js
route-miss deadletter, and the Electron main-process intent path while loading
runtime `2.3.0` generation `a83ab412`.

Run the candidate and registry gates from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh --require-public-metadata
scripts/verify-npm-registry-candidates.sh
```
