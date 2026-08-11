# npm Runtime 2.4.0 Candidate

Status: ready for npm publish.

This directory records the Node.js, Bun, and Electron runtime `2.4.0`
candidates from connector source `0afb5e9`, payload staging commit `ef40ada`,
and native generation `2.4.0+c2f53117`:

- `coakka-v2-connector-node@2.4.0`
- `coakka-v2-connector-bun@2.4.0`
- `coakka-v2-connector-electron@2.4.0`

Node.js and Bun contain the exact Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64 native payloads. Electron depends on exact Node.js `2.4.0`.

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/0afb5e9/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/0afb5e9/manifest.json \
  --dry-run
```

After publishing, verify registry metadata and exact tarball digests:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/0afb5e9/manifest.json
```
