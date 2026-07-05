# 2026-07-05 Runtime Non-JVM c124a9e Refresh 66ebe58

This note records the public non-JVM runtime connector refresh over native
package generation `0.2.0+c124a9e` for connector source commit `66ebe58`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| Python runtime | `coakka_v2_connector` wheel | `0.2.0` | `0.2.0+c124a9e` |
| Node.js runtime | `coakka-v2-connector-node` package | `0.2.0` | `0.2.0+c124a9e` |
| Go runtime | `coakka-v2-connector-go` source package | `0.2.0` | `0.2.0+c124a9e` |
| C# runtime | `CoAkka.Runtime` NuGet package | `0.2.0` | `0.2.0+c124a9e` |
| Rust runtime | `coakka-runtime-rs` package | `0.2.0-spike` | `0.2.0+c124a9e` |
| Mojo runtime | `coakka-runtime-mojo` source package | `0.2.0-source` | `0.2.0+c124a9e` |
| Zig runtime | `coakka-runtime-zig` source package | `0.2.0-source` | `0.2.0+c124a9e` |

This refresh aligns all current public non-JVM runtime lanes with the same
native package generation already used by the current public native C ABI,
runtime JVM jar, and JVM framework adapters. The connector refresh updates
packaging metadata, staged native expectations, runtime library resolution, and
public consumer docs so the public surface no longer mixes `94a5729` and
`c124a9e` generations across language lanes.

User-facing changes:

- Python, Node.js, Go, C#, and Rust packages now advertise and resolve native
  package `0.2.0+c124a9e` consistently.
- Python integration tests now prefer packaged or staged repo-local runtime
  libraries before falling back to host-local build outputs, avoiding stale
  dylib linkage accidents during source-owner verification.
- Mojo and Zig now publish source packages that carry their own
  `CONSUMING.md`, release metadata, and packaged native runtime payloads for
  `macos-aarch64`, `linux-aarch64`, and `linux-x86_64`.
- Mojo and Zig packaged smokes now prefer packaged native libraries first, then
  fall back to repo-local staging only for source checkout work.

Validation covered:

- Python packaged-wheel smoke and full unit/integration suite
- Node.js packaged-tarball smoke and repo-local test suite
- Go packaged-source smoke and repo-local `go test ./...`
- C# packaged NuGet smoke
- Rust repo-local tests plus extracted-package smoke
- Mojo extracted source-package smoke
- Zig extracted source-package smoke
- runtime intake gate for all seven public artifacts
- public artifact manifest and surface verification in `coakka-publish`
