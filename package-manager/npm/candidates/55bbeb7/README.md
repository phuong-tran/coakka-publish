# npm Candidate 55bbeb7

This directory stages npm-ready candidate tarballs for the JavaScript runtime
and logger lanes. It is not a public package-manager release claim.

Status: blocked on npm registry account, package ownership, and publish
credentials.

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

After `npm adduser` succeeds, publish in the manifest order:

```sh
scripts/publish-npm-package-manager-candidates.sh --publish
```

After npm publishing is complete, refresh the compatibility matrix, public
artifact manifest, samples, and release notes to name the registry
coordinates as current.
