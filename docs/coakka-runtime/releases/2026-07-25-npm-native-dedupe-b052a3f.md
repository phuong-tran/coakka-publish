# 2026-07-25 npm Native Alias Dedupe b052a3f

This npm patch release cleans the JavaScript runtime/logger package tarballs
after the npm package browser showed both generation-pinned native libraries
and unversioned aliases for the same platform.

Status: published to npm and registry-verified on 2026-07-25.

Candidate coordinates:

- `coakka-v2-connector-node@1.3.8`
- `coakka-v2-connector-bun@1.3.8`
- `coakka-v2-connector-electron@1.3.8`
- `coakka-logger-node@1.2.6`
- `coakka-logger-bun@1.2.6`
- `coakka-logger-electron@1.2.6`

Native generations are unchanged:

- runtime native: `1.3.1+0da8c2d9`
- logger native: `1.2.1+f50756ebff0d`

The package resolver still accepts unversioned native names for explicit
local/dev paths, but published npm tarballs now ship only one native library per
platform: the generation-pinned file. The package-manager verifier now fails if
Node.js or Bun tarballs contain both the generation-pinned native and the
unversioned alias for the same platform.

Verification:

```sh
scripts/test-npm-package-manager-artifact.sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/b052a3f/manifest.json
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/b052a3f/manifest.json \
  --publish \
  --otp <current-code>
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/b052a3f/manifest.json
```

Manual tarball inspection confirms the candidate runtime/logger Node.js and Bun
tarballs list only versioned native file names under each supported platform.
