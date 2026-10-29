# npm JavaScript Runtime 1.4.5

Published the CoAkka Node.js, Bun, and Electron runtime packages at `1.4.5`
on August 3, 2026:

- `coakka-v2-connector-node@1.4.5`
- `coakka-v2-connector-bun@1.4.5`
- `coakka-v2-connector-electron@1.4.5`

## Artifact Identity

- Connector source: `2c0c598e9cc42bfb34556cee5016b7ca255114fd`
- Native generation: `1.4.0+2cee86bf`
- Native source: `2cee86bf89f38725aa763bb3abd96b64a904875d`
- Node.js tarball SHA-256:
  `c3464cd05b28368587779890503d3e9ec4af71b59af9bb4bec571f412297c7b0`
- Bun tarball SHA-256:
  `2b75cfe6d8d7b2d3ce1eaf2d087f0ec777af58275f9446561bf9bd564664a866`
- Electron tarball SHA-256:
  `d3c1948ca3428de369472fbc432daf3724d1836bccdc93b76429bc63e47a1ad9`

The npm package version advances independently from the native runtime
version. No native binary, public C ABI, native checksum, runtime capability,
or signing state changed. Node.js and Bun embed the same verified native bytes
for Linux ARM64, macOS ARM64, and Windows x86-64; Electron consumes the exact
Node.js package coordinate.

## Package Boundary

Node.js and Bun expose their documented package root through explicit package
exports. Public transport envelope and deadletter types remain available from
that root. Binary framing and control-envelope helpers remain internal
implementation details and cannot be imported as package subpaths.

This boundary change does not alter transport bytes or request/reply behavior.
The framing implementation remains required by native generation
`1.4.0+2cee86bf` and stays owned by the connector package.

## Verification

The release passed Node.js tests, Bun native request/reply tests, packaged
Node.js/Bun/Electron consumer smokes, exact three-platform payload checks,
package export checks, npm publish dry-runs, connector hosted CI run
`30764304372`, and publish hosted CI run `30764418505`.

After publication, npm reported the exact local tarball SHA-1 values,
dependencies, license metadata, and `latest` dist-tags. Clean registry installs
then completed:

- Node.js package-boundary and native request/reply smoke
- Bun package-boundary and native request/reply smoke
- Electron main/preload/renderer bridge request/reply smoke

The loaded runtime reported version `1.4.0`, source `2cee86bf`, TCP backend,
and capability mask `221`.

## License And Trust

The packages carry CoAkka Public Artifact License 1.1. Publisher signing
remains absent and is not a package or runtime gate. Platform-specific loader
and trust guidance remains in the canonical troubleshooting document.

Install exact versions:

```sh
npm install coakka-v2-connector-node@1.4.5
npm install coakka-v2-connector-bun@1.4.5
npm install coakka-v2-connector-electron@1.4.5
```
