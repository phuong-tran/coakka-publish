# CoAkka Runtime Packages

**Application-owned work across languages and processes.**

CoAkka Runtime is a **polyglot, cross-platform distributed runtime for
application-owned work**, backed by a **shared native core**. Its native-backed
language connectors expose familiar APIs while keeping business behavior in
application code.

Application work often crosses a language or process boundary: a web API asks
a Python worker to process an image, or an existing JVM service handles a
customer operation. CoAkka Runtime gives that work stable target names and
a shared way to send requests, receive replies and observe delivery failures.

The application chooses where its handlers live and keeps its business rules.
Runtime handles the communication between those participants. This makes it
useful for connecting existing systems and evolving an application gradually,
including moving work from a local handler into another process.

Read [The CoAkka Story](../docs/coakka-runtime/explanation/coakka-story.md) for
why internal application work is modeled this way, or
[try one example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/README.md)
to see the model in code.

Install the connector for your application's language. Its package supplies
the matching native Runtime; applications normally do not install Core separately.

## Published packages

The [published package ledger](../docs/common/current-packages.md) owns exact
versions and coordinates. **C# `CoAkka.Runtime 3.0.0` is available on NuGet**
and **Python `coakka-v2-connector==3.0.0` on PyPI**. Other 3.0.0 Runtime
connectors remain local candidates until their individual
publication. Follow the ledger for the particular language you use.

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
