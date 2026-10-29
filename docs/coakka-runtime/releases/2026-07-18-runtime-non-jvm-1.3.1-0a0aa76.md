# 2026-07-18 Runtime Non-JVM 1.3.1 Refresh 0a0aa76

This note records the public non-JVM runtime connector refresh over native
package generation `1.3.1+bda2ef5` for connector source commit `0a0aa76`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| Python runtime | `coakka_v2_connector` wheel | `1.3.1` | `1.3.1+bda2ef5` |
| Node.js runtime | `coakka-v2-connector-node` package | `1.3.1` | `1.3.1+bda2ef5` |
| Go runtime | `coakka-v2-connector-go` source package | `1.3.1` | `1.3.1+bda2ef5` |
| C# runtime | `CoAkka.Runtime` NuGet package | `1.3.1` | `1.3.1+bda2ef5` |
| Rust runtime | `coakka-runtime-rs` package | `1.3.1-spike` | `1.3.1+bda2ef5` |
| Mojo runtime | `coakka-runtime-mojo` source package | `1.3.1-source` | `1.3.1+bda2ef5` |
| Zig runtime | `coakka-runtime-zig` source package | `1.3.1-source` | `1.3.1+bda2ef5` |

This refresh keeps the current non-JVM runtime family aligned on the same
native package generation as the public C ABI archive and JVM runtime while
rebundling all seven public language lanes from one connector source commit.

macOS and Windows are both supported development/validation hosts in this
refresh. The published artifacts in this note bundle the current
macOS/Linux/Windows native payload set, while Linux remains the default
deployment path for runtime rollout claims.

Validation covered:

- Python packaged-wheel build plus runtime intake verification
- Node.js packaged-tarball test/build plus runtime intake verification
- Go packaged-source tests/build plus runtime intake verification
- C# packaged NuGet build plus runtime intake verification
- Rust test/package build plus runtime intake verification
- Mojo source package build plus runtime intake verification
- Zig source package build plus runtime intake verification
- publish-side `scripts/verify-public-surface.sh`
