# npm Runtime 2.5.2 Candidate

Status: ready for npm publication after release approval and credentials.

This directory freezes the Node.js, Bun, and Electron Runtime `2.5.2`
packages built from connector commit `3ae74f4` over unchanged native
generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`:

- `coakka-v2-connector-node@2.5.2`
- `coakka-v2-connector-bun@2.5.2`
- `coakka-v2-connector-electron@2.5.2`

This corrective train exposes replica-pinned File and Stream Lane owner grants
through the connector surface, validates control-plane reconstruction, and
keeps bearer grants out of Electron preload and renderer APIs. Native behavior,
ABI, protocol, and native bytes are unchanged. Node.js and Bun carry the exact
five-platform native payload; Electron depends on exact Node.js `2.5.2`.

Verify without publishing:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/3ae74f4/manifest.json

scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/3ae74f4/manifest.json \
  --dry-run
```

Publication must use the frozen tarballs in this directory in Node.js, Bun,
then Electron order. Registry verification and receipt metadata are added only
after the uploads succeed.
