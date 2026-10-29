# PyPI Python Runtime 2.1.0

Published `coakka-v2-connector==2.1.0` to PyPI on August 9, 2026.

## Artifact Identity

- Connector source: `34d15e78426550709897e758af495b500bde1044`
- Native generation: `2.1.0+60ddf70d`
- Native source: `60ddf70d63b94750bb76c8284923e73199788c2e`
- Wheel SHA-256:
  `fb9b06a060336f3d663fe5241f1a6085a138ca55c3b9d0b6c64abce287195bf0`

The wheel contains generic and versioned native runtime libraries for Linux
ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64. Every payload matches the
promoted native artifact digest.

## Registry Verification

Before upload, the package suite passed 29 tests, 8 skips, and 4 subtests. The
packaged consumer loaded runtime `2.1.0+60ddf70d` and completed request/reply.
Twine metadata validation passed.

After publication, the exact wheel URL from the PyPI JSON API downloaded with
the same SHA-256 as the staged candidate. The downloaded wheel passed the same
readiness, native digest, public documentation, and clean consumer smoke.

The package README links to the canonical public file-lane contract. Readiness
rejects links to non-public connector or core repositories.

Install:

```sh
python -m pip install coakka-v2-connector==2.1.0
```
