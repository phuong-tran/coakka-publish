# npm Candidate ae6472b

This directory stages npm-ready candidate tarballs for the public metadata and
onboarding patch release.

Status: staged and release-ready. This candidate has not been published to npm.

The candidate packages are built from connector commit `ae6472b`. This patch
keeps the runtime stop-backpressure native generation, fixes npm package page
metadata so it does not point at the private connector source repository, and
aligns first-run onboarding around the fake backend HTTP to CoAkka target
story.

Native generations:

- runtime native: `1.3.1+0da8c2d9`
- logger native: `1.2.1+f50756ebff0d`

Candidate coordinates:

- `coakka-v2-connector-node@1.3.7`
- `coakka-v2-connector-bun@1.3.7`
- `coakka-v2-connector-electron@1.3.7`
- `coakka-logger-node@1.2.5`
- `coakka-logger-bun@1.2.5`
- `coakka-logger-electron@1.2.5`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/ae6472b/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/ae6472b/manifest.json \
  --dry-run
```

Publish with current npm 2FA:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/ae6472b/manifest.json \
  --publish \
  --otp <current-code>
```

Then verify the registry surface:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/ae6472b/manifest.json
```
