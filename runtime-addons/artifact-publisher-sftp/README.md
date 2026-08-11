# SFTP Artifact Publisher Runtime Addon

Status: release contract and native package template only. There is currently
no public SFTP addon archive or package coordinate in this repository.

The addon composes one external acquisition workflow with the existing CoAkka
Runtime File Lane:

```text
Service A app host
  -> SFTP acquisition with pinned host identity
  -> size and SHA-256 verification
  -> verified no-clobber local publication
  -> Service A sender File Lane
  -> Service B receiver File Lane
```

The application host owns credentials, authorization grants, lifecycle, retry
policy, and business rollout decisions. The addon owns SFTP mechanics and the
bounded fetch-to-send workflow. CoAkka Runtime continues to own File Lane
delivery semantics.

## Planned Native Coordinate

```text
runtime-addons/artifact-publisher-sftp/native/releases/<version>+<source>/
  coakka-runtime-addon-artifact-publisher-sftp-native-<version>.tar.gz
```

The addon version is independent from the runtime version. The first release
must declare runtime ABI major `2`, a minimum runtime version that includes
File Lane, and `file_lane` as a required runtime feature.

## Release Gates

- static absorption of libssh2 and its crypto/compression closure;
- no user-installed SFTP, crypto, or compression library;
- exact reviewed C ABI export list;
- matching-host runtime-module execution for every packaged platform;
- host-key mismatch, credential rejection, integrity failure, destination
  collision, receiver rejection, partial fan-out, and cancellation evidence;
- safe staging and crash-durability documentation for each platform;
- archive intake through `scripts/verify-runtime-addon-release.py`;
- public sample promotion only after the archive is listed in
  `artifacts/public-artifacts.tsv`.

Windows must not appear in a release manifest until its staging implementation
is handle-relative and defends against reparse-point traversal. A dependency
cross-build alone is not runtime evidence.

The native package template lives under
[`native/package-template`](native/package-template/README.md).
