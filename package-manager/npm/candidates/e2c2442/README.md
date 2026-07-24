# npm Candidate e2c2442

This directory stages npm-ready candidate tarballs for an npm package metadata
links patch release.

Status: candidate prepared; blocked on npm publish OTP.

The candidate packages are built from connector commit `e2c2442`. This patch
adds npm metadata links (`repository`, `homepage`, `bugs`, and `keywords`) so
npm package pages point directly to the connector source and first-run samples.
Native runtime and logger generations remain:

- runtime native: `1.3.1+bda2ef5`
- logger native: `1.2.1+f50756ebff0d`

Candidate coordinates:

- `coakka-v2-connector-node@1.3.5`
- `coakka-v2-connector-bun@1.3.5`
- `coakka-v2-connector-electron@1.3.5`
- `coakka-logger-node@1.2.4`
- `coakka-logger-bun@1.2.4`
- `coakka-logger-electron@1.2.4`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/e2c2442/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/e2c2442/manifest.json \
  --dry-run
```

Publish with current npm 2FA:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/e2c2442/manifest.json \
  --publish \
  --otp <current-code>
```

Then verify the registry surface:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/e2c2442/manifest.json
```
