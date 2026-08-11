# SFTP Artifact Publisher Native 0.2.1

Release generation: `0.2.1+c5656cc8`

This release supersedes `0.2.0`; that package is retained only in its tagged
GitHub Release for audit history and must not be distributed.

Published archive:

```text
coakka-runtime-addon-artifact-publisher-sftp-native-0.2.1.tar.gz
```

This release supports `linux-aarch64`, `linux-x86_64`, `macos-aarch64`,
`windows-aarch64`, and `windows-x86_64`. It requires CoAkka Runtime native
`2.3.0` or newer with Runtime ABI major `2` and the public File Lane feature.
The archive does not contain another Runtime copy.

## Evidence

- Core source snapshot: `c5656cc8`
- libssh2: `1.11.1`, statically absorbed on all targets
- crypto: OpenSSL `3.6.2` on macOS, OpenSSL `3.5.7` on Linux, OS WinCNG on
  Windows
- public C ABI export list: exactly 11 reviewed symbols on every target
- matching-host module workflow: passed on all five targets
- SFTP acquisition, SHA-256 verification, no-clobber staging, failure and
  cancellation paths, and File Lane delivery: passed on all five targets
- Linux ARM64/x86-64: ASan plus UBSan and TSan passed
- dynamic dependency audit: no ambient libssh2, libcrypto, or libssl dependency
- packaged modules contain no build-host paths; the macOS module has no build
  RPATH

Windows staging uses directory-handle-relative temporary creation and
no-clobber hard-link publication and rejects reparse roots. The Windows DLLs
are not Authenticode-signed; verify the release checksums and apply an
organization signature when local application-control policy requires one.

Linux modules carry `DF_1_NODELETE`. Publisher instances support stop and
destroy, but unloading and later reloading the addon module is not supported.

Verify this release directory with:

```sh
python3 scripts/verify-runtime-addon-release.py \
  --release-dir runtime-addons/artifact-publisher-sftp/native/releases/0.2.1+c5656cc8 \
  --expected-addon artifact-publisher-sftp
```
