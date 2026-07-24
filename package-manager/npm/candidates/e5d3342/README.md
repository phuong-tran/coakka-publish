# npm Candidate e5d3342

This directory stages npm-ready candidate tarballs for the JavaScript runtime
and logger onboarding patch release.

Status: published to npm and registry-verified.

The candidate packages are built from connector commit `e5d3342`. This is a
package README and first-reader onboarding patch release only; the native
runtime and logger generations remain:

- runtime native: `1.3.1+bda2ef5`
- logger native: `1.2.1+f50756ebff0d`

Candidate coordinates:

- `coakka-v2-connector-node@1.3.2`
- `coakka-v2-connector-bun@1.3.2`
- `coakka-v2-connector-electron@1.3.2`
- `coakka-logger-node@1.2.2`
- `coakka-logger-bun@1.2.2`
- `coakka-logger-electron@1.2.2`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/e5d3342/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/e5d3342/manifest.json \
  --dry-run
```

The packages were published to npm on 2026-07-25. If this release ever needs a
resume check, the publish helper skips already-published verified packages:

```sh
scripts/publish-npm-package-manager-candidates.sh --publish --otp <current-code>
```

Then verify the registry surface:

```sh
scripts/verify-npm-registry-candidates.sh
```
