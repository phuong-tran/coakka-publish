# npm Candidate 55bbeb7

This directory records the npm candidate tarballs used to publish the
JavaScript runtime and logger lanes.

Status: published to npm and registry-verified on 2026-07-24.

The candidate packages are built from connector commit `55bbeb7`; the matching
sample harness commit is `923f86e`.

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh --dry-run
```

The release was published in the manifest order:

```sh
scripts/publish-npm-package-manager-candidates.sh --publish
```

For accounts with publish 2FA, pass the current authenticator code:

```sh
scripts/publish-npm-package-manager-candidates.sh --publish --otp 123456
```

Verify the registry surface:

```sh
scripts/verify-npm-registry-candidates.sh
```

The published npm coordinates are:

- `coakka-v2-connector-node@1.3.1`
- `coakka-v2-connector-bun@1.3.1`
- `coakka-v2-connector-electron@1.3.1`
- `coakka-logger-node@1.2.1`
- `coakka-logger-bun@1.2.1`
- `coakka-logger-electron@1.2.1`
