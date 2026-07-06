# 2026-07-06 Runtime Non-JVM 1.2.1 Refresh fa29f94

This note records the public non-JVM runtime connector refresh over native
package generation `1.2.1+abde383` for connector source commit `fa29f94`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| Python runtime | `coakka_v2_connector` wheel | `1.2.1` | `1.2.1+abde383` |
| Node.js runtime | `coakka-v2-connector-node` package | `1.2.1` | `1.2.1+abde383` |
| Go runtime | `coakka-v2-connector-go` source package | `1.2.1` | `1.2.1+abde383` |
| C# runtime | `CoAkka.Runtime` NuGet package | `1.2.1` | `1.2.1+abde383` |
| Rust runtime | `coakka-runtime-rs` package | `1.2.1-spike` | `1.2.1+abde383` |
| Mojo runtime | `coakka-runtime-mojo` source package | `1.2.1-source` | `1.2.1+abde383` |
| Zig runtime | `coakka-runtime-zig` source package | `1.2.1-source` | `1.2.1+abde383` |

This refresh keeps the current non-JVM runtime family aligned on the same
native package generation as the public C ABI archive and JVM runtime while
rebundling all seven public language lanes from one connector source commit.

macOS and Windows are both supported development/validation hosts in this
refresh. The published artifacts in this note bundle the current
macOS/Linux/Windows native payload set, while Linux remains the default
deployment path for runtime rollout claims.

Validation covered:

- Python packaged-wheel smoke plus runtime intake verification
- Node.js packaged-tarball smoke plus runtime intake verification
- Go packaged-source smoke plus runtime intake verification
- C# packaged NuGet smoke plus sample-consumer `StartLocal(...)` path
- Rust packaged archive build plus runtime intake verification
- Mojo source smoke, packaged archive build, and runtime intake verification
- Zig source smoke, packaged archive build, and runtime intake verification
- publish-side `scripts/verify-public-surface.sh`
