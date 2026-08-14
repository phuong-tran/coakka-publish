# SFTP Artifact Publisher Runtime Addon

Status: native `1.2.0+88b9a047` is public for `linux-aarch64`, `linux-x86_64`,
`macos-aarch64`, `windows-aarch64`, and `windows-x86_64`. It requires CoAkka
Runtime native `2.3.0` or newer with File Lane support.

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

## Native Coordinate

```text
runtime-addons/artifact-publisher-sftp/native/releases/1.2.0+88b9a047/
  coakka-runtime-addon-artifact-publisher-sftp-native-1.2.0.tar.gz
```

The addon version is independent from the runtime version. This release
declares runtime ABI major `2`, minimum native Runtime `2.3.0`, and `file_lane`
as its required Runtime feature.

## Release Gates

- static absorption of libssh2 and its crypto/compression closure;
- no user-installed SFTP, crypto, or compression library;
- exact reviewed C ABI export list;
- matching-host runtime-module execution for every packaged platform;
- host-key mismatch, credential rejection, integrity failure, destination
  collision, receiver rejection, partial fan-out, and cancellation evidence;
- safe staging and crash-durability documentation for each platform;
- archive intake through `scripts/verify-runtime-addon-release.py`;
- public sample consumption of the exact archive listed in
  `artifacts/public-artifacts.tsv`.

Release consumers should use the public binary archive: it absorbs libssh2 and
the target crypto implementation and avoids a compiler/dependency toolchain on
the deployment host. The implementation source remains in the private Core
workspace and is not represented as a public download. Public consumers receive
the reviewed C header, package manifest, license notices, checksums, audit
notes, and a buildable two-process integration sample.

The Windows DLLs are not Authenticode-signed. Standard Windows execution does
not universally require a publisher signature, but SmartScreen, Defender
Application Control, or a managed allow-list policy may require one. Verify
the release SHA-256 values before use; organizations that require signed code
must sign the verified DLLs with their own trusted certificate and policy.

The native package template lives under
[`native/package-template`](native/package-template/README.md).

The independent
[native sample](https://github.com/phuong-tran/coakka-samples/tree/main/runtime-addons/artifact-publisher-sftp/native)
consumes the immutable addon and Runtime archives, then exercises SFTP
acquisition plus cross-process File Lane delivery. The root sample runner keeps
it under the distinct `runtime-addons` command.
