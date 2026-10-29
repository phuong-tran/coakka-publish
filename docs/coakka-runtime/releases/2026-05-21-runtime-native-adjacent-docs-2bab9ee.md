# 2026-05-21 Runtime Native-Adjacent Connector Docs

This note records the C#, Zig, and Mojo connector documentation refresh for
source connector commit `2bab9ee`.

| Surface | Public artifact | Version |
| --- | --- | --- |
| C# runtime | `CoAkka.Runtime` NuGet package | `0.2.0` from `0.2.0+94a5729-2bab9ee` |
| Zig runtime | `coakka-runtime-zig` source package | `0.2.0-source` from `0.2.0+c124a9e-2bab9ee` |
| Mojo runtime | `coakka-runtime-mojo` source package | `0.2.0-source` from `0.2.0+c124a9e-2bab9ee` |

C# keeps native package generation `0.2.0+94a5729`. Zig and Mojo keep native
package generation `0.2.0+c124a9e`. The refresh changes connector source,
documentation, package metadata, and samples; it does not introduce a new
native runtime binary.

User-facing changes:

- C# XML documentation is generated into the NuGet package for public APIs.
- C# now exposes local-first helpers: `RuntimeHost.StartLocal`,
  `RuntimeHost.LocalRoute`, `RegisterTextHandler`, `AskTextAsync`, and
  `PayloadIdentity.Text`.
- Zig public structs/functions carry inline `///` docs and expose
  `localStartSpec(...)` for first-run source examples.
- Mojo docs now explicitly describe the lane as a low-level native smoke, not a
  stable application connector API.

Validation covered:

- C# package build and packaged smoke in `connectorKotlin`
- Zig source smoke in `connectorKotlin`
- Mojo source smoke in `connectorKotlin`
- runtime intake gate for C#, Zig, and Mojo public artifacts
- public artifact manifest and surface verification in this repository
