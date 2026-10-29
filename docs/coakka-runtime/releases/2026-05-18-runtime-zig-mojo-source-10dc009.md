# CoAkka Runtime Mojo And Zig Source Connector Release 10dc009

This note records the first public source connector packages for Mojo and Zig.

| Surface | Artifact | Version | Native package |
| --- | --- | --- | --- |
| Mojo runtime source connector | `runtime/mojo/releases/0.2.0+c124a9e-10dc009/coakka-runtime-mojo-0.2.0-source.tar.gz` | `0.2.0-source` | `0.2.0+c124a9e` |
| Zig runtime source connector | `runtime/zig/releases/0.2.0+c124a9e-10dc009/coakka-runtime-zig-0.2.0-source.tar.gz` | `0.2.0-source` | `0.2.0+c124a9e` |

The packages bundle the scanner-clean public native runtime libraries for:

- `linux-aarch64`
- `linux-x86_64`
- `macos-aarch64`

The source connector proof covers:

- lifecycle/control startup
- local route snapshot apply
- raw request/reply through the delivered-request lane
- route-miss deadletter through the ask-client path
- runtime version/git diagnostics

Mojo and Zig remain source connector packages in this release. They are public
evaluation artifacts and sample execution inputs, not package-manager releases
yet.

Validation covered:

- runtime intake gate for `mojo` and `zig`
- public artifact surface scanner
- unpacked package smoke on macOS `aarch64`
- release `SHA256SUMS`
- root artifact manifest verification
