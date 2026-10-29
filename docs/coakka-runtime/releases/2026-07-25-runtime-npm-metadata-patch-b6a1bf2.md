# 2026-07-25 Runtime npm Metadata Patch b6a1bf2

This release republishes the JavaScript runtime npm packages so public package
version constants match the package versions.

The native runtime generation is unchanged: `1.3.1+bda2ef5`.

Published npm coordinates:

- `coakka-v2-connector-node@1.3.3`
- `coakka-v2-connector-bun@1.3.3`
- `coakka-v2-connector-electron@1.3.3`

The staged candidate set lives under
`package-manager/npm/candidates/b6a1bf2/` and passes:

```sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/b6a1bf2/manifest.json
scripts/verify-npm-registry-candidates.sh --manifest package-manager/npm/candidates/b6a1bf2/manifest.json
```
