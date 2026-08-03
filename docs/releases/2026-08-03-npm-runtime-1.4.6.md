# npm JavaScript Runtime 1.4.6

Published the CoAkka Node.js, Bun, and Electron runtime packages at `1.4.6`
on August 3, 2026:

- `coakka-v2-connector-node@1.4.6`
- `coakka-v2-connector-bun@1.4.6`
- `coakka-v2-connector-electron@1.4.6`

## Artifact Identity

- Connector source: `37816322`
- Native generation: `1.4.1+9e02a51d`
- Native source: `9e02a51d7f0e4a231e2f71fe6d19ce02724277c9`
- Node.js tarball SHA-256:
  `4c4f5d1970aba43641a13dda9ec752ed49a1e0c41f1f8821b47294d6ef462f85`
- Bun tarball SHA-256:
  `e18d24444e16a68b9381482d36f9313ceb3211225eb48ad0a2d38bfd4c064e84`
- Electron tarball SHA-256:
  `31dc6cf8070b7f3c39076132218bb2fbec8c9aadf7b72655acd0a1da60720a71`

The npm package version advances independently from the native runtime
version. Node.js and Bun embed the same verified native bytes for Linux ARM64,
Linux x86-64, macOS ARM64, Windows ARM64, and Windows x86-64. Electron
consumes the exact Node.js package coordinate.

## Verification

The release passed the candidate gate under
`package-manager/npm/candidates/37816322/`, exact five-platform payload
checks, npm publish, and post-publish registry verification.

After publication, npm reported these registry shasums and integrity values:

- Node.js: `1a3aebe36e064a7dc9e830aa392cb862f88e7bb0`
- Bun: `33d193e2a9f3a45433ade51fdfe9c39e55d8a803`
- Electron: `054b764fe0b9f5adb570a21d5e4948b687be3225`

Registry verification confirms that `latest` now resolves to `1.4.6` for all
three runtime lanes.

Install exact versions:

```sh
npm install coakka-v2-connector-node@1.4.6
npm install coakka-v2-connector-bun@1.4.6
npm install coakka-v2-connector-electron@1.4.6
```
