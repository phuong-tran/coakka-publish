# npm Candidate f3e25ec

This directory stages npm-ready candidate tarballs for a package README
first-run smoke patch release.

Status: candidate prepared; blocked on npm publish OTP.

The candidate packages are built from connector commit `f3e25ec`. This patch
adds a no-checkout first npm smoke path to npm package README pages. Native
runtime and logger generations remain:

- runtime native: `1.3.1+bda2ef5`
- logger native: `1.2.1+f50756ebff0d`

Candidate coordinates:

- `coakka-v2-connector-node@1.3.4`
- `coakka-v2-connector-bun@1.3.4`
- `coakka-v2-connector-electron@1.3.4`
- `coakka-logger-node@1.2.3`
- `coakka-logger-bun@1.2.3`
- `coakka-logger-electron@1.2.3`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/f3e25ec/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/f3e25ec/manifest.json \
  --dry-run
```

Publish with current npm 2FA:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/f3e25ec/manifest.json \
  --publish \
  --otp <current-code>
```

Then verify the registry surface:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/f3e25ec/manifest.json
```
