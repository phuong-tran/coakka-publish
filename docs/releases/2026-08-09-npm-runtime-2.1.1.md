# npm JavaScript Runtime 2.1.1

Published a documentation patch for all three JavaScript runtime connectors:

- `coakka-v2-connector-node@2.1.1`
- `coakka-v2-connector-bun@2.1.1`
- `coakka-v2-connector-electron@2.1.1`

The `2.1.0` README linked its file-lane contract to the non-public connector
source repository and used stale wording that implied a future native runtime
was required. `2.1.1` links to the canonical public contract at
[`docs/runtime-file-transfer.md`](../runtime-file-transfer.md) and states the
bundled native generation exactly: `2.1.0+60ddf70d`.

The runtime implementation and five-platform native payloads are unchanged.
Node.js and Bun include Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64. Electron depends on exact Node.js `2.1.1`.

Verification completed before and after publication:

- package boundary and public metadata verification for all three tarballs;
- packaged README assertion for the canonical URL and rejection of the
  non-public connector URL;
- Node.js and Bun clean packaged request/reply through native runtime `2.1.0`;
- Electron packaged main-process intent smoke;
- registry `latest=2.1.1` and exact registry shasum match for every tarball;
- unauthenticated HTTP `200` for the canonical file-lane contract.

The immutable receipt, registry integrity values, and tarballs are under
[`package-manager/npm/candidates/80cdd77/`](../../package-manager/npm/candidates/80cdd77/).
