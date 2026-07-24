# npm Candidate 8ff6f32

This directory stages npm-ready runtime candidate tarballs for the stop
backpressure hotfix.

Status: staged locally, not published.

The candidate packages are built from connector commit `8ff6f32` and bundle
runtime native generation `1.3.1+0da8c2d9`.

Candidate coordinates:

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

Publish later with current npm 2FA:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/8ff6f32/manifest.json \
  --publish \
  --otp <current-code>
```
