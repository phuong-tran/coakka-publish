# PyPI Python Runtime 2.3.0

Published `coakka-v2-connector==2.3.0` to PyPI on August 10, 2026.

## Artifact Identity

- Connector source: `3a84c7b658e97d4aa989283103cc561c89420685`
- Native generation: `2.3.0+a83ab412`
- Native source: `a83ab41207821388eb5507be63d81ba721d6d585`
- Wheel SHA-256:
  `dae933c5ba842236ca4ab4d8ca24ac7d0b75aa964036ac41cdb0eff282b2fd81`

The wheel contains generic and versioned native runtime libraries for Linux
ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64. Every payload matches the
promoted native artifact digest. The Python surface includes request/reply,
deadletters, route hot reload, File Lane, and Stream Lane.

## Registry Verification

Before upload, the exact candidate passed 30 tests, 9 skips, 4 subtests, the
packaged request/reply smoke, metadata and Twine checks, plus five focused
File Lane and Stream Lane tests on macOS ARM64.

After publication, the PyPI JSON API reported version `2.3.0`, the expected
filename, `yanked=false`, and the same SHA-256 as the candidate. The wheel
downloaded from `files.pythonhosted.org` is byte-identical to the staged wheel.
A clean install from `https://pypi.org/simple` completed request/reply, matched
deadletter, route hot reload, and local desktop CRUD against native source
`a83ab412`.

Install:

```sh
python -m pip install coakka-v2-connector==2.3.0
```
