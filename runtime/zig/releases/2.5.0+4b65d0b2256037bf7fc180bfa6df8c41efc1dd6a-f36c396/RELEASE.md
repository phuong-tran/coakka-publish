# Releasing The Zig Connector

This source package is `2.5.0-source` and uses native
generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.

Required dry-run gates:

```sh
bash scripts/check-platform-source.sh
bash scripts/package-release.sh
bash scripts/check-package-readiness.sh
```

The archive must contain exactly the digest-locked macOS ARM64, Linux ARM64,
and Windows x86-64 natives. Linux/Windows compile/link and payload presence do
not replace connector/native execution there. Publisher signing is absent.
