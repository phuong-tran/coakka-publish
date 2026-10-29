# CoAkka Runtime Packages

Install the connector for your application's language. Its package supplies
the matching native Runtime; applications normally do not install Core separately.

## Published packages

The [published package ledger](../docs/common/current-packages.md) owns exact
versions and coordinates. Runtime 3.0.0 is being prepared; **it has not been
uploaded to package registries**. Follow the ledger's released versions until
that status changes for the particular language you use.

| Application | Distribution and installation reference |
| --- | --- |
| Node.js, Bun, Electron | [npm coordinates and versions](../docs/common/current-packages.md) |
| Python | [PyPI coordinate and version](../docs/common/current-packages.md) |
| C# | [NuGet coordinate and version](../docs/common/current-packages.md) |
| Kotlin / Java, Spring Boot, Quarkus | [Maven artifacts](../maven/README.md) |
| Go / Swift | [Go modules and SwiftPM](../docs/common/current-packages.md) |
| C/C++ or source/archive consumers | [Native archive catalog](../runtime/native/README.md) and [connector coordinates](../docs/common/current-packages.md) |

[Run the matching published example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/README.md)
when trying a package. That page separates released samples from the 3.0.0 candidates.

## Candidate and platform scope

Use the [3.0.0 candidate index](../docs/coakka-runtime/previews/README.md) for
local candidate artifacts. The intended five desktop targets are macOS ARM64,
Linux ARM64/x64 and Windows ARM64/x64. See [package/platform evidence](../docs/coakka-runtime/reference/runtime-package-platform-evidence.md)
for each package's actual version and support; do not assume an old package
inherits this candidate's targets. Android and Mojo remain previews. Swift's
Windows ARM lane is preview; that does not make the whole Swift package preview.

## Tools, extensions and reference

[Client and Inspect](../coakka-tools/README.md), [native Runtime tests](runtime-test/README.md)
and [optional addons](../runtime-addons/README.md) have their own catalogs.
For configuration and operation, start with [Runtime documentation](../docs/coakka-runtime/README.md).
[Release history](../docs/coakka-runtime/releases/README.md) records older identities.
[Choose another product](../README.md).
