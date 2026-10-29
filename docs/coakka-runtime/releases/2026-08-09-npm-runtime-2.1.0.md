# npm JavaScript Runtime 2.1.0

Published the CoAkka Node.js, Bun, and Electron runtime packages at `2.1.0`
on August 9, 2026:

- `coakka-v2-connector-node@2.1.0`
- `coakka-v2-connector-bun@2.1.0`
- `coakka-v2-connector-electron@2.1.0`

## Artifact Identity

- Connector source: `4782dcd`
- Native generation: `2.1.0+60ddf70d`
- Native source: `60ddf70d63b94750bb76c8284923e73199788c2e`
- Node.js SHA-256: `770e1db352c00918b01ef01545d3e8f83326ff9f2da746bcad01e375f8e842ff`
- Bun SHA-256: `775702aec53838647176438cac8d3443462585be95f7198b5600c7900eed9331`
- Electron SHA-256: `7175efe1eedb79625e5800b129cad5eaaacaf579a1a56952b1da2637c9b316d5`

Node.js and Bun package all five native platforms. Electron depends on exact
Node.js connector version `2.1.0`. All three expose the file-lane API.

## Registry Verification

npm reports `latest=2.1.0` for all three packages. Registry downloads match
the staged tarballs byte-for-byte by SHA-256. Clean Node.js and Bun installs
pass their package-surface checks and native request/reply smoke while loading
runtime `2.1.0`, full source commit `60ddf70d63b94750bb76c8284923e73199788c2e`.
A clean Electron install resolves both connectors at `2.1.0` and completes
the Electron main-process intent request/reply smoke.

The complete receipt, npm shasums, integrity values, and immutable tarballs
are under `package-manager/npm/candidates/4782dcd/`.

```sh
npm install coakka-v2-connector-node@2.1.0
npm install coakka-v2-connector-bun@2.1.0
npm install coakka-v2-connector-electron@2.1.0
```
