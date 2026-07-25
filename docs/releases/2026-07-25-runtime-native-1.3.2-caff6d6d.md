# 2026-07-25 Runtime Native 1.3.2 caff6d6d

This note records the additive native runtime cut for JavaScript native payload
bridge work.

| Surface | Artifact | Version |
| --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.2+caff6d6d/coakka-runtime-native-v2-1.3.2.tar.gz` | `1.3.2+caff6d6d` |

This cut adds native transport envelope/deadletter codec helpers so JavaScript
runtime connectors can stop shipping JavaScript protobuf or hand-written wire
codecs. The runtime C ABI version remains `1`; older callers that do not use
the new helper symbols remain on the existing ABI contract.

Published platform coverage in this package is:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

Validation covered:

- focused transport codec regression tests in `coakkaCoreNativeDev`
- Linux `aarch64` and `x86_64` public native package staging in Docker
- macOS `aarch64` public native rebuild
- Windows `aarch64` and `x86_64` public DLL builds
- native dependency checks proving no blocked public shared dependency leaks
- deterministic archive verification with
  `v2/scripts/verify_public_native_release.sh`

The language package-manager follow-up is intentionally separate. npm packages
published before this cut, including runtime connector `1.3.8`, still point at
the older native runtime generation and must be superseded by a later npm patch
before they can consume these new helper symbols.
