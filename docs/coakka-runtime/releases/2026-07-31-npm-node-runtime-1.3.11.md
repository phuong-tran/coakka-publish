# npm Node.js Runtime 1.3.11

Published `coakka-v2-connector-node@1.3.11` to npm on July 31, 2026.

## Runtime Generation

The package embeds public native runtime generation:

```text
1.3.4+dc6ec284
```

Packaged native coverage:

- macOS ARM64
- Linux ARM64
- Linux x86_64
- Windows ARM64
- Windows x86_64

## Verification

The published coordinate was installed into a clean temporary Node.js
project. The consumer smoke loaded the embedded native runtime and completed a
local request/reply round trip:

```text
consumer_runtime_info abi=1 version=1.3.4 git=dc6ec284 backend=tcp
consumer_runtime_smoke ok payload=node-package-ok version=1.3.4 backend=tcp
```

The package tarball also passed a private-path marker scan. Build-only Koffi
`.exp` and `.lib` files are excluded; the vendored runtime surface retains the
platform-specific `koffi.node` modules required at execution time.

## Scope

This release advances only the Node.js runtime package. It does not imply that
the Bun or Electron runtime packages have advanced from `1.3.9`.

Install:

```sh
npm install coakka-v2-connector-node@1.3.11
```
