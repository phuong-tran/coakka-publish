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

After npm publishing is complete, refresh the compatibility matrix, public
artifact manifest, samples, and release notes to name the registry
coordinates as current.
