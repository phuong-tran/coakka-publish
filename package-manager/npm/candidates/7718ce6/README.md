# npm JavaScript And Desktop Compatibility Candidate

Status: staged on August 18, 2026; npm publication remains gated.

This directory records six packages from connector source `7718ce6`:

- Runtime Node.js, Bun, and Electron `2.4.1` over native
  `2.4.0+c2f53117`.
- Logger Node.js, Bun, and Electron `1.2.7` over native
  `1.2.1+f50756ebff0d`.

Node.js requires version 22 or newer, Bun requires 1.2.0 or newer, and
Electron requires version 42 or newer. Electron delegates native ownership to
its exact first-party Node package. Node.js and Bun contain five native
platform payloads; their macOS ARM64 runtime and logger binaries are checksum
pinned and declare deployment target 13.0.

Run the candidate and release dry-run gates from the repository root:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/7718ce6/manifest.json

scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/7718ce6/manifest.json \
  --dry-run
```

After publication, verify the registry metadata and exact tarball bytes:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/7718ce6/manifest.json
```
