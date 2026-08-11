# SFTP Artifact Publisher Native 1.1.0

Release generation: `1.1.0+42841ae2`

Published archive:

```text
coakka-runtime-addon-artifact-publisher-sftp-native-1.1.0.tar.gz
```

This release supports `linux-aarch64`, `linux-x86_64`, `macos-aarch64`,
`windows-aarch64`, and `windows-x86_64`. It requires CoAkka Runtime native
`2.3.0` or newer with Runtime ABI major `2` and the public File Lane feature.
The archive does not contain another Runtime copy.

## Audit Corrections

- POSIX cancellation pipes are now required to be nonblocking and
  close-on-exec; initialization fails closed if descriptor setup fails.
- POSIX no-clobber publication now reports storage failure if its temporary
  hard-link name cannot be removed.
- The public C ABI documentation now describes the implemented Windows
  directory-handle and reparse-point protection instead of the retired
  fail-closed candidate behavior.
- Product version is `1.1.0`; ABI `SOVERSION` remains `0`.

## Evidence

- Private implementation snapshot: `42841ae2`
- libssh2: `1.11.1`, statically absorbed on every target
- public C ABI export list: exactly 11 reviewed symbols
- macOS ARM64: exact-source SFTP-to-File-Lane fixture, C ABI, and manifest tests
  passed after the audit corrections
- Linux ARM64/x86-64: strict exact-source build, public C ABI smoke, and the
  full SFTP-to-File-Lane fixture passed on matching-host containers after the
  audit corrections
- Windows ARM64/x86-64: strict exact-source cross-build passed; the prior
  Windows 11 matching-host workflow remains the native runtime evidence
- dynamic dependency intake rejects ambient libssh2, libcrypto, or libssl
- packaged modules contain no build-host paths; Linux carries `DF_1_NODELETE`

Windows DLLs remain unsigned. Verify SHA-256 before use and apply an
organization signature when local application-control policy requires one.

The implementation source is not a public download. The public package carries
the reviewed header, manifest, notices, checksums, this evidence statement, and
an independent buildable integration sample.

Verify this release directory with:

```sh
python3 scripts/verify-runtime-addon-release.py \
  --release-dir runtime-addons/artifact-publisher-sftp/native/releases/1.1.0+42841ae2 \
  --expected-addon artifact-publisher-sftp
```
