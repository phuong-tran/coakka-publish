# CoAkka Runtime Packages

[Language examples](#start-in-your-language) · [Packages](#published-packages) · [Platform scope](#candidate-and-platform-scope) · [Addons](#runtime-addons) · [Documentation](../docs/coakka-runtime/README.md) · [All products](../README.md)

**Application-owned work across languages and processes.**

CoAkka Runtime is a **polyglot, cross-platform distributed runtime** backed by a
**shared native core**. Its native-backed language packages connect named pieces
of application work while your application keeps its business rules and data.

For example, a Node.js checkout can ask a Python pricing function for a quote.
Runtime carries the request and reply; Python calculates the price. Start with
one local example in your language, then try a two-process application.

Read [the CoAkka story](../docs/coakka-runtime/explanation/coakka-story.md) for
why the work belongs to the application. The normal connector package includes
its matching native Runtime; no separate Core installation is needed.

## Languages, frameworks and desktop apps

Runtime integrations include **JavaScript and TypeScript (Node.js / Bun),
Python, Java, Kotlin, C#, Go and Swift**, with **Spring Boot** and **Quarkus**
framework adapters and **Electron** desktop support. The language table below
leads to the released 3.0.0 first runs.

**Tauri**, C/C++, Rust and Zig use version-specific source/archive guides;
Android and Mojo remain previews. Choose the exact version and platform in
those guides rather than applying one package's support scope to another.

## Start in your language

Choose one **published 3.0.0** example. Its guide gives installation, a run
command, expected output and the application source, including how to close it.

| Your application | Runnable example |
| --- | --- |
| Node.js | [Node.js first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/node/registry-basic/README.md) |
| Bun | [Bun first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/bun/registry-basic/README.md) |
| Electron | [Electron first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/electron/registry-basic/README.md) |
| Python | [Python first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/python/registry-basic/README.md) |
| C# | [C# first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/csharp/registry-basic/README.md) |
| Kotlin | [Kotlin first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/jvm/registry-basic/README.md#kotlin) |
| Java | [Java first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/jvm/registry-basic/README.md#java) |
| Go | [Go first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/go/registry-basic/README.md) |
| Swift | [Swift first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/swift/registry-basic/README.md) |
| Spring Boot | [Spring Boot first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/scenarios/customer-crud/framework-registry/README.md#spring-boot) |
| Quarkus | [Quarkus first example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/scenarios/customer-crud/framework-registry/README.md#quarkus) |

For other hosts, see [language and evaluation paths](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/README.md#other-languages-and-evaluations).
Use the [package ledger](../docs/common/current-packages.md) for exact install
coordinates, or the catalogs below when choosing an archive.

## Published packages

The [published package ledger](../docs/common/current-packages.md) owns exact
versions and coordinates. **C# `CoAkka.Runtime 3.0.0` is available on NuGet**
and **Python `coakka-v2-connector==3.0.0` on PyPI**. **Node.js, Bun and Electron
3.0.0 are available on npm**. **Go `/v3@v3.0.0` and Swift 3.0.0 are released on GitHub**. **JVM Runtime, Spring Boot and Quarkus 3.0.0 are published on Maven Central**.
Other Runtime 3.0.0 lanes remain local candidates until their own publication. Follow the ledger for the particular language you use.

| Application | Distribution and installation reference |
| --- | --- |
| Node.js, Bun, Electron | [npm coordinates and versions](../docs/common/current-packages.md) |
| Python | [PyPI coordinate and version](../docs/common/current-packages.md) |
| C# | [NuGet coordinate and version](../docs/common/current-packages.md) |
| Kotlin / Java, Spring Boot, Quarkus | [Maven Central 3.0.0](../docs/coakka-runtime/releases/2026-10-30-jvm-spring-quarkus-runtime-3.0.0-maven-central.md) |
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

## Runtime addons

**Runtime addons** add optional capabilities around CoAkka Runtime. The released
**Artifact Source Addons** obtain and verify files from external sources before
Runtime transfers those files between application peers.

For example, acquire an exact model file from **S3/MinIO** or **Hugging Face Hub**,
verify it, then deliver it to the worker that needs it. Other provider guides
cover HTTPS, SFTP, cloud storage and local files.

Current addon packages expose a **native C interface**. Each addon has its own
version, supported platforms and Runtime compatibility; high-level language
addon bindings are not currently released. Add only the capability you need.

[Understand the workflow](../docs/coakka-runtime/guides/runtime-addons.md) ·
[Choose a provider example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/addons/README.md) ·
[Package and compatibility catalog](../runtime-addons/README.md).

## Runtime companion tools

- [CoAkka Runtime Client](../docs/coakka-runtime/operations/coakka-runtime-client.md)
  (`coakka-runtime-client`): try requests from the terminal or a script. The
  published command is `coakka-client`.
- [CoAkka Runtime Inspect](../docs/coakka-runtime/operations/coakka-runtime-inspect.md)
  (`coakka-runtime-inspect`): explore Runtime facts and try a request in a browser.

The tool guides keep their own release and inspection scope. Their version is
independent of the connector package selected for your application.

## Tools, extensions and reference

[Client and Inspect](../coakka-tools/README.md), [native Runtime tests](runtime-test/README.md)
and [optional addons](../runtime-addons/README.md) have their own catalogs.
For configuration and operation, start with [Runtime documentation](../docs/coakka-runtime/README.md).
[Release history](../docs/coakka-runtime/releases/README.md) records older identities.
[Choose another product](../README.md).

Read the [Runtime 3.0.0 changelog](../docs/coakka-runtime/CHANGELOG.md) before upgrading older application code.
