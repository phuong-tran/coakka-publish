# npm Candidate 8ff6f32

This directory stages npm-ready runtime candidate tarballs for the stop
backpressure hotfix.

Status: published to npm and registry-verified on 2026-07-25.

The candidate packages are built from connector commit `8ff6f32` and bundle
runtime native generation `1.3.1+0da8c2d9`.

Published coordinates:

- `coakka-v2-connector-node@1.3.6`
- `coakka-v2-connector-bun@1.3.6`
- `coakka-v2-connector-electron@1.3.6`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/8ff6f32/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/8ff6f32/manifest.json \
  --dry-run
```

Verify the published registry artifacts:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/8ff6f32/manifest.json
```
