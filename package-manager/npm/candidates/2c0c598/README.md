# npm Runtime Candidate 2c0c598

This directory stages the npm-ready JavaScript runtime packages for native
runtime generation `1.4.0+2cee86bf`.

Status: ready for npm publish.

The candidate tarballs are byte-identical copies of the artifacts already
published under the Node.js, Bun, and Electron runtime release directories.
They were built from connector commit `2c0c598` and contain only the promoted
native platform matrix:

- Linux ARM64
- macOS ARM64
- Windows x86-64

Candidate coordinates:

- `coakka-v2-connector-node@1.4.5`
- `coakka-v2-connector-bun@1.4.5`
- `coakka-v2-connector-electron@1.4.5`

The package patch keeps native generation `1.4.0+2cee86bf` unchanged. Node and
Bun expose only their documented package root; binary transport framing stays
internal and is not a supported deep-import surface.

Run the candidate gate:

```sh
scripts/test-npm-package-manager-candidates.sh \
  package-manager/npm/candidates/2c0c598/manifest.json
```

Run the npm publish dry-run:

```sh
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/2c0c598/manifest.json \
  --dry-run
```

After publication, verify registry metadata, tarball SHA-1, dependencies, and
the `latest` dist-tag:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/2c0c598/manifest.json
```
