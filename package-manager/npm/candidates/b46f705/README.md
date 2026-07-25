# npm Candidate b46f705

This directory records npm-ready runtime connector tarballs for the native
runtime `1.3.2` bridge release.

Status: ready for npm publish.

The candidate packages are built from connector commit `b46f705` and bundle
native runtime generation `1.3.2+caff6d6d`. The JavaScript runtime no longer
ships its own protobuf transport codec; Node.js and Bun call the native payload
bridge exported by the bundled runtime library. Electron delegates native
runtime loading through the first-party Node package.

Candidate coordinates:

- `coakka-v2-connector-node@1.3.9`
- `coakka-v2-connector-bun@1.3.9`
- `coakka-v2-connector-electron@1.3.9`

Run the candidate gate from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/b46f705/manifest.json
```

Run the release dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/b46f705/manifest.json \
  --dry-run
```

Verify the published registry surface after publishing:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/b46f705/manifest.json
```
