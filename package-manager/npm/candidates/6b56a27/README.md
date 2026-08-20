# npm Runtime 2.5.0 Candidate

Status: staged on August 20, 2026; npm publication remains gated.

This directory records the Node.js, Bun, and Electron Runtime `2.5.0`
packages rebuilt from clean connector commit `6b56a27`, payload staging commit
`eb62ec8`, and native generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`:

- `coakka-v2-connector-node@2.5.0`
- `coakka-v2-connector-bun@2.5.0`
- `coakka-v2-connector-electron@2.5.0`

Node.js and Bun contain the exact Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64 native payloads. Electron depends on exact Node.js `2.5.0`.
Every package carries Apache `LICENSE`, `NATIVE-LICENSE.md`, the package-scope
`PACKAGE-LICENSE.md`, and `NOTICE`. Runtime README sections appear in the
required File Lane, Stream Lane, and AI-Assisted Integration order.

Run the candidate and release dry-run gates from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/6b56a27/manifest.json

scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/6b56a27/manifest.json \
  --dry-run
```

After publication, verify registry metadata and exact tarball bytes:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/6b56a27/manifest.json
```
