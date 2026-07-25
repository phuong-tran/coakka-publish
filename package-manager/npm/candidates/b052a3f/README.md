# npm Candidate b052a3f

This directory stages npm-ready candidate tarballs for the native alias dedupe
patch release.

Status: staged, not published.

The candidate packages are built from connector commit `b052a3f`. This patch
keeps the same runtime and logger native generations, but removes duplicate
unversioned native library aliases from the Node.js and Bun package tarballs.
The package resolver still accepts unversioned library names for explicit
local/dev paths; the published npm tarballs ship only the generation-pinned
native file for each platform.

Native generations:

- runtime native: `1.3.1+0da8c2d9`
- logger native: `1.2.1+f50756ebff0d`

Candidate coordinates:

- `coakka-v2-connector-node@1.3.8`
- `coakka-v2-connector-bun@1.3.8`
- `coakka-v2-connector-electron@1.3.8`
- `coakka-logger-node@1.2.6`
- `coakka-logger-bun@1.2.6`
- `coakka-logger-electron@1.2.6`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/b052a3f/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/b052a3f/manifest.json \
  --dry-run
```
