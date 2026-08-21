# Releasing The Zig Connector

This source package is `2.5.3-source` and uses native
generation `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`.

Required dry-run gates:

```sh
bash scripts/check-platform-source.sh
bash scripts/package-release.sh
bash scripts/check-package-readiness.sh
```

The archive must contain exactly the digest-locked macOS ARM64, Linux ARM64,
and Windows x86-64 natives. Linux/Windows compile/link and payload presence do
not replace connector/native execution there. Publisher signing is absent.
