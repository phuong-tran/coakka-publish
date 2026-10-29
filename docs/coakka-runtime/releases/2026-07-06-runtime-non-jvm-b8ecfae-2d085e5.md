# 2026-07-06 Runtime Non-JVM b8ecfae Refresh 2d085e5

This note records the public non-JVM runtime connector refresh over native
package generation `0.2.0+b8ecfae` for connector source commit `2d085e5`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| Python runtime | `coakka_v2_connector` wheel | `0.2.0` | `0.2.0+b8ecfae` |
| Node.js runtime | `coakka-v2-connector-node` package | `0.2.0` | `0.2.0+b8ecfae` |
| Go runtime | `coakka-v2-connector-go` source package | `0.2.0` | `0.2.0+b8ecfae` |
| C# runtime | `CoAkka.Runtime` NuGet package | `0.2.0` | `0.2.0+b8ecfae` |
| Rust runtime | `coakka-runtime-rs` package | `0.2.0-spike` | `0.2.0+b8ecfae` |
| Mojo runtime | `coakka-runtime-mojo` source package | `0.2.0-source` | `0.2.0+b8ecfae` |
| Zig runtime | `coakka-runtime-zig` source package | `0.2.0-source` | `0.2.0+b8ecfae` |

This refresh keeps the current non-JVM runtime family aligned on the same
native package generation as the public C ABI archive and JVM runtime while
rebundling all seven public language lanes from one connector source commit.

macOS and Windows are both supported development/validation hosts in this
refresh. The published artifacts in this note now bundle the current
macOS/Linux/Windows native payload set, while Linux remains the default
deployment path for runtime rollout claims.

User-facing changes:

- Python, Node.js, Go, C#, and JVM resolvers no longer fall back to internal
  native-library names in the public path.
- Python, Node.js, Go, C#, Rust, Mojo, and Zig package payloads now carry both
  `windows-aarch64` and `windows-x86_64` runtime DLLs in addition to the
  existing macOS/Linux set.
- C# local helpers continue to auto-select one free IPv4 loopback port when
  `RuntimeHost.StartLocal(...)` or `RuntimeHost.LocalRoute(...)` omits
  `diagnosticPort` or sets it to `0`.
- Rust, Mojo, and Zig consuming guides keep public-source wording while
  following the same native package truth as the other language lanes.

Validation covered:

- Python packaged-wheel smoke plus runtime intake verification
- Node.js packaged-tarball smoke plus runtime intake verification
- Go packaged-source smoke plus runtime intake verification
- C# packaged NuGet smoke plus sample-consumer `StartLocal(...)` path
- Rust source smoke, packaged archive build, and runtime intake verification
- Mojo source smoke, packaged archive build, and runtime intake verification
- Zig source smoke, packaged archive build, and runtime intake verification
- publish-side `scripts/verify-public-surface.sh`
