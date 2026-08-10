# Releasing The Zig Connector

This source package is `2.3.0-source` and uses native
generation `2.3.0+a83ab412`.

Required dry-run gates:

```sh
bash scripts/check-platform-source.sh
bash scripts/package-release.sh
bash scripts/check-package-readiness.sh
```

The archive must contain exactly the digest-locked macOS ARM64, Linux ARM64,
and Windows x86-64 natives. Linux/Windows compile/link and payload presence do
not replace connector/native execution there. Publisher signing is absent.
