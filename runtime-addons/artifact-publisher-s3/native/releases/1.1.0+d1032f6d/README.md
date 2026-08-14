# S3 Artifact Publisher Native 1.1.0

Release generation: `1.1.0+d1032f6d`

Published archive: `coakka-runtime-addon-artifact-publisher-s3-native-1.1.0.tar.gz`

Supported platforms: `linux-aarch64`, `linux-x86_64`, `macos-aarch64`, `windows-aarch64`, `windows-x86_64`. Minimum Runtime native version: `2.4.0`.

## Evidence

- exact private implementation snapshot: `d1032f6d`;
- exact 11-symbol reviewed C ABI;
- strict build and public C ABI execution on every listed matching host;
- dynamic dependency and sibling-module loader inspection on packaged binaries;
- bounded worker/queue, cancellation, timeout, integrity, and no-clobber tests;
- archive structure, checksum, installed manifest, and CMake intake verification.

This archive contains no Runtime copy and no high-level connector. Windows DLLs
are unsigned; verify SHA-256 and apply an organization signature when required.

```sh
python3 scripts/verify-runtime-addon-release.py \
  --release-dir runtime-addons/artifact-publisher-s3/native/releases/1.1.0+d1032f6d \
  --expected-addon artifact-publisher-s3
```
