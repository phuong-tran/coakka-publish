# npm Candidate b6a1bf2

This directory stages npm-ready candidate tarballs for a runtime package
metadata patch release.

Status: published to npm and registry-verified.

The candidate packages are built from connector commit `b6a1bf2`. This patch
corrects runtime package version constants exposed by the JavaScript runtime
packages. The native runtime generation remains `1.3.1+bda2ef5`.

Candidate coordinates:

- `coakka-v2-connector-node@1.3.3`
- `coakka-v2-connector-bun@1.3.3`
- `coakka-v2-connector-electron@1.3.3`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/b6a1bf2/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/b6a1bf2/manifest.json \
  --dry-run
```

The packages were published to npm on 2026-07-25. If this release ever needs a
resume check, the publish helper skips already-published verified packages:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/b6a1bf2/manifest.json \
  --publish \
  --otp <current-code>
```

Then verify the registry surface:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/b6a1bf2/manifest.json
```
