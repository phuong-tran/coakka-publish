# CoAkka Runtime Native Refresh b8ecfae

This note records the native runtime `0.2.0+b8ecfae` public refresh.

| Surface | Artifact | Version |
| --- | --- | --- |
| Native C ABI | `runtime/native/releases/0.2.0+b8ecfae/coakka-runtime-native-v2-0.2.0.tar.gz` | `0.2.0+b8ecfae` |

The refresh keeps the public runtime surface self-contained while moving the
native package truth to source snapshot `b8ecfae`. The macOS `aarch64` dylib in
this cut also keeps a valid `LC_ID_DYLIB` install name, so the same public
binary can be embedded and loaded by downstream JVM packaging instead of only
passing dependency and surface scans.

Python, Node.js, Go, C#, Rust, Mojo, and Zig remain on the earlier
`0.2.0+c124a9e` runtime refresh in this repository. This native refresh only
changes the public C ABI package and the runtime JVM family that is explicitly
rebundled against it.

Validation covered:

- Linux `aarch64` and `x86_64` public native package staging
- macOS `aarch64` public native rebuild with loadable `LC_ID_DYLIB`
- native dependency checks and public surface scanning
- deterministic native archive refresh and publish-manifest update

