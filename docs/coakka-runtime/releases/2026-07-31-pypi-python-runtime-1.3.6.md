# PyPI Python Runtime 1.3.6

Published `coakka-v2-connector==1.3.6` to PyPI on July 31, 2026.

## Runtime Generation

The wheel embeds public native runtime generation:

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

The published coordinate was installed into a clean temporary virtualenv. The
consumer smoke loaded the embedded native runtime and completed a local
request/reply round trip:

```text
consumer_runtime_info abi=1 version=1.3.4 git=dc6ec284 backend=tcp
consumer_runtime_smoke ok payload=maven-local-ok version=1.3.4 backend=tcp
```

The wheel digest reported by PyPI matches the locally verified release wheel:

```text
sha256:7d31b14fc4d571790a18473575f7965fb93eef59dea43ab662f22c99aa3bce8b
```

Install:

```sh
python -m pip install coakka-v2-connector==1.3.6
```
