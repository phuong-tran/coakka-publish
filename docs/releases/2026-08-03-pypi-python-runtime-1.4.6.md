# PyPI Python Runtime 1.4.6

Published `coakka-v2-connector==1.4.6` to PyPI on August 3, 2026.

## Artifact Identity

- Python package version: `1.4.6`
- Connector source: `603a3728`
- Native generation: `1.4.1+9e02a51d`
- Native source: `9e02a51d7f0e4a231e2f71fe6d19ce02724277c9`
- Wheel SHA-256:
  `09285c8310a44e3c4e590239d478a2dab48e7ab1a2abc940884690dab1db43cf`

The wheel contains exactly Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64 native libraries.

## Verification

Before upload, the exact checksum-locked wheel passed `27` tests with `6`
capability/platform skips and `4` subtests, packaged-wheel request/reply smoke,
metadata/platform/digest validation, and `twine check`. The publish entrypoint
verified that the wheel did not change during those gates.

After publication, the PyPI JSON API reported version `1.4.6`, the exact wheel
SHA-256 above, and `yanked=false`. A clean isolated install from the PyPI simple
index imported the package from its temporary `site-packages` directory, loaded
runtime `1.4.1` from the full native source commit, and completed request/reply.

Install:

```sh
python -m pip install coakka-v2-connector==1.4.6
```
