# npm Runtime 2.5.1 Candidate

Status: ready for npm publication after release approval and credentials.

This directory freezes the Node.js, Bun, and Electron Runtime `2.5.1`
packages built from connector commit `11c1555` over unchanged native
generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`:

- `coakka-v2-connector-node@2.5.1`
- `coakka-v2-connector-bun@2.5.1`
- `coakka-v2-connector-electron@2.5.1`

The patch corrects public documentation links and applies Native Artifact
License 1.2 by file scope. Runtime behavior, ABI, protocol, and native bytes
are unchanged. Node.js and Bun carry the exact five-platform native payload;
Electron depends on exact Node.js `2.5.1`.

Verify without publishing:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/11c1555/manifest.json

scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/11c1555/manifest.json \
  --dry-run
```

Publication must use the frozen tarballs in this directory in Node.js, Bun,
then Electron order. Registry verification and receipt metadata are added only
after the uploads succeed.
