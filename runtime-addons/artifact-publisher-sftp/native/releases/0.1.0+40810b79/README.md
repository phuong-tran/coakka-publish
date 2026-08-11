# SFTP Artifact Publisher Native 0.1.0

Release generation: `0.1.0+40810b79`

Published archive:

```text
coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz
```

This first independent addon release supports `macos-aarch64`. It requires
CoAkka Runtime native `2.3.0` or newer with Runtime ABI major `2` and the public
File Lane feature. The archive does not contain another Runtime copy.

## Evidence

- Core source snapshot: `40810b79`
- Release build: AppleClang 17, CMake `Release`
- packaged deployment target: macOS `26.0`
- libssh2: `1.11.1`, statically absorbed
- OpenSSL crypto: `3.6.2`, statically absorbed
- macOS system zlib: `1.2.12`
- public C ABI export list: 11 reviewed symbols
- matching-host module workflow: passed
- packaged CMake consumer against Runtime `2.3.0`: passed
- SFTP acquisition, SHA-256 verification, no-clobber staging, and cross-process
  File Lane delivery: passed
- dynamic dependency audit: no ambient libssh2, libcrypto, or libssl dependency

Linux is not advertised by this release because matching-host package evidence
has not yet been recorded. Windows remains fail-closed until staging uses
directory-handle-relative I/O with reparse-point defense.

Verify this release directory with:

```sh
python3 scripts/verify-runtime-addon-release.py \
  --release-dir runtime-addons/artifact-publisher-sftp/native/releases/0.1.0+40810b79 \
  --expected-addon artifact-publisher-sftp
```
