# 2026-07-05 Runtime Non-JVM c124a9e Refresh c4be778

This note records the public non-JVM runtime connector refresh over native
package generation `0.2.0+c124a9e` for connector source commit `c4be778`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| Python runtime | `coakka_v2_connector` wheel | `0.2.0` | `0.2.0+c124a9e` |
| Node.js runtime | `coakka-v2-connector-node` package | `0.2.0` | `0.2.0+c124a9e` |
| Go runtime | `coakka-v2-connector-go` source package | `0.2.0` | `0.2.0+c124a9e` |
| C# runtime | `CoAkka.Runtime` NuGet package | `0.2.0` | `0.2.0+c124a9e` |
| Rust runtime | `coakka-runtime-rs` package | `0.2.0-spike` | `0.2.0+c124a9e` |
| Mojo runtime | `coakka-runtime-mojo` source package | `0.2.0-source` | `0.2.0+c124a9e` |
| Zig runtime | `coakka-runtime-zig` source package | `0.2.0-source` | `0.2.0+c124a9e` |

This refresh keeps the current non-JVM runtime family aligned on the same
native package generation while rebundling all seven public language lanes from
one connector source commit. The native/JVM `b8ecfae` refresh stays separate;
this note does not claim non-JVM compatibility with `0.2.0+b8ecfae`.

User-facing changes:

- Python, Node.js, Go, and C# resolvers no longer fall back to internal
  `libcoakka_runtime_v2_caf.*` names in the public path.
- C# local helpers now auto-select one free IPv4 loopback port when
  `RuntimeHost.StartLocal(...)` or `RuntimeHost.LocalRoute(...)` omits
  `diagnosticPort` or sets it to `0`.
- Rust, Mojo, and Zig consuming guides now use public-source wording instead of
  internal trial language while keeping their current package/version truth.

Validation covered:

- Python packaged-wheel smoke plus runtime intake verification
- Node.js packaged-tarball smoke plus runtime intake verification
- Go packaged-source smoke plus runtime intake verification
- C# packaged NuGet smoke plus sample-consumer `StartLocal(...)` path
- Rust source smoke, packaged archive build, and runtime intake verification
- Mojo source smoke, packaged archive build, and runtime intake verification
- Zig source smoke, packaged archive build, and runtime intake verification
- publish-side `scripts/verify-public-surface.sh`
