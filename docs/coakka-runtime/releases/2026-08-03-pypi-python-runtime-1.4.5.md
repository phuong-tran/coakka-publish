# PyPI Python Runtime 1.4.5

Published `coakka-v2-connector==1.4.5` to PyPI on August 3, 2026.

## Artifact Identity

- Python package version: `1.4.5`
- Connector source: `77c9db848c51231bc138b3999d87af3766d30b01`
- Native generation: `1.4.0+2cee86bf`
- Native source: `2cee86bf89f38725aa763bb3abd96b64a904875d`
- Wheel SHA-256:
  `082bfcda88e7b3fe850f9402d06ab79cd3e970636547d79305db66215eacfa2a`

The package version advances independently from the native runtime version.
No native binary, public ABI, native checksum, capability, or signing state
changed. The wheel embeds the same verified native bytes distributed in the
public runtime 1.4.0 artifact train for:

- Linux ARM64
- macOS ARM64
- Windows x86-64

## Verification

The source package passed `27` tests with `6` capability/platform skips and
`4` subtests, packaged-wheel request/reply smoke, `twine check`, the public
runtime intake gate, and connector hosted CI run `30762821035`.

After publication, the PyPI JSON API reported the same wheel SHA-256 as the
locally verified candidate. A clean virtualenv installed the exact package
from `pypi.org`, loaded runtime version `1.4.0` from source `2cee86bf`, observed
capability mask `221`, and completed request/reply:

```text
consumer_runtime_info abi=1 version=1.4.0 git=2cee86bf89f38725aa763bb3abd96b64a904875d backend=tcp capabilities=221
consumer_runtime_smoke ok payload=maven-local-ok version=1.4.0 backend=tcp capabilities=221
```

## License And Trust

The wheel carries CoAkka Public Artifact License 1.1. Publisher signing remains
absent and is not a runtime or package gate. Platform-specific loader and trust
guidance remains in the canonical troubleshooting document.

Install:

```sh
python -m pip install coakka-v2-connector==1.4.5
```
