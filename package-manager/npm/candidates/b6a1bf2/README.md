# npm Candidate b6a1bf2

This directory stages npm-ready candidate tarballs for a runtime package
metadata patch release.

Status: candidate prepared; blocked on npm publish OTP.

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

Publish with current npm 2FA:

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
