# CoAkka Publish

<p align="center"><img src="docs/common/assets/brand/coakka-logo.png" alt="CoAkka" width="480"></p>

[Website](https://coakka-ecosystem-preview.vivid-plume-2955.chatgpt.site/) · [Find a package](#choose-your-product) · [Documentation](docs/README.md) · [HTTP benchmarks](coakka-http-runtime/benchmark/)

**Contents**

[Choose a product](#choose-your-product) · [CoAkka Runtime](#coakka-runtime-languages-frameworks-and-app-hosts) · **[CoAkka HTTP Runtime](#coakka-http-runtime-shared-http-familiar-languages)** · [HTTP Inspect](#coakka-http-runtime-inspect) · [Runtime addons](#coakka-runtime-addons) · [Runtime Client / Inspect](#coakka-runtime-client-and-inspect) · [Node–Python demo](#try-the-nodepython-web-demo)

**Shared native cores. Familiar APIs across languages.**

CoAkka is a **polyglot, cross-platform ecosystem** built on shared native cores.
**CoAkka Runtime** is a **native-backed distributed runtime for
application-owned work**: for example, a Node.js checkout asking Python to
calculate a price. Your application keeps its handlers, business rules and data;
Runtime carries the request and reply across languages, processes and hosts.

Use **HTTP Runtime** for HTTP services and **Logger** for logging with bounded
storage. Each product can be adopted independently and has its own packages
and versions.

**[Find a package by product and language](#choose-your-product)**

## Try the Node–Python web demo

Build a basket in your browser. Node.js sends it to a separate Python process,
which returns item prices, shipping and the total through CoAkka Runtime.

**[Run the demo and read the walkthrough](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/scenarios/order-quote-node-python/README.md)** · Published npm/PyPI **3.0.0** packages · Two terminals, no Docker required.

[![Node–Python demo: change the basket and receive an updated quote from Python.](docs/coakka-runtime/assets/node-python-order-quote-demo.gif)](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/scenarios/order-quote-node-python/README.md)

Change the quantities, request a quote and see Python return the updated total.
[View a still screenshot](docs/coakka-runtime/assets/node-python-order-quote-web.png).

## Choose your product

| I need… | First steps | Package catalog |
| --- | --- | --- |
| Calls between application work across processes or languages | [Runtime in my language](docs/coakka-runtime/getting-started.md) | [Runtime packages](coakka-runtime/README.md) |
| An HTTP API, web frontend or streaming endpoint | [HTTP Runtime in my language](docs/coakka-http-runtime/getting-started.md) | [HTTP archives](coakka-http-runtime/README.md) |
| Application logging and visible pressure | [Logger in my language](docs/coakka-logger/README.md#start-with-one-consumer) | [Logger packages](coakka-logger/README.md) |

## CoAkka Runtime: languages, frameworks and app hosts

CoAkka Runtime brings application-owned work to backend, desktop and device
applications through three complementary lanes:

### Message-Lane, File-Lane and Stream-Lane

| What your application needs | Runtime lane | Learn more |
| --- | --- | --- |
| Send commands, ask for results or publish events | **Message-Lane** | [Requests, replies and routing](docs/coakka-runtime/explanation/runtime-message-and-routing-model.md) |
| Move a stable file that already exists on the sending host | **File-Lane** | [File transfer](docs/coakka-runtime/guides/runtime-file-transfer.md) |
| Send live frames as they are produced, such as camera, audio or telemetry | **Stream-Lane** | [Streaming](docs/coakka-runtime/guides/runtime-streaming.md) |

Choose the integration that matches your code:

| Application language or host | Start here |
| --- | --- |
| JavaScript / TypeScript on Node.js and Bun; Python; Java and Kotlin; C#; Go; Swift | [Language-first Runtime examples](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/README.md#first-run-choose-your-language) |
| Spring Boot and Quarkus | [Framework customer applications](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/scenarios/customer-crud/framework-registry/README.md) |
| Electron and Tauri desktop apps | [Electron](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/electron/registry-basic/README.md) · [Tauri](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/tauri/README.md) |
| C / C++, Rust, Zig, Android and Mojo | [Source/archive guides and previews](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/README.md#other-languages-and-evaluations) |

Start with the **3.0.0** language examples. Tauri, C/C++, Rust and Zig use their
own versioned source/archive guides; Android and Mojo are previews. Follow each
guide's package, lane and platform scope.

## CoAkka Runtime addons

**Artifact Source Addons** acquire and verify an exact model file from
**S3/MinIO**, **Hugging Face Hub**
or another provider, then deliver it through **File-Lane**. Already have a stable
local file? Use File-Lane directly.

**Native C is the maintained addon interface.** Other language bindings depend
on concrete user demand. Addons keep their own versions and platform scope;
check [Runtime 3.0.0 compatibility](docs/coakka-runtime/guides/native-addons-runtime3.md)
before selecting one.

[Understand the workflow](docs/coakka-runtime/guides/runtime-addons.md) ·
[Choose a provider example](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/addons/README.md) ·
[Package and compatibility catalog](runtime-addons/README.md).

## CoAkka Runtime Client and Inspect

Try requests before writing another application, or inspect a request during
integration. Product names and released commands are listed separately:

| Product | Released command | Use it for |
| --- | --- | --- |
| [CoAkka Runtime Client](docs/coakka-runtime/operations/coakka-runtime-client.md) | `coakka-client` | Send requests from a terminal or script and inspect the result. |
| [CoAkka Runtime Inspect](docs/coakka-runtime/operations/coakka-runtime-inspect.md) | `coakka-runtime-inspect` | Explore Runtime facts and try request/reply in a browser. |

Each guide records the tool's version, connection setup and inspection scope.

## CoAkka HTTP Runtime: shared HTTP, familiar languages

**CoAkka HTTP Runtime** (`coakka-http-runtime`) provides a shared native HTTP
core for **polyglot HTTP servers and outbound HTTP requests**. Build APIs, web
frontends, uploads, file delivery, streaming and WebSocket services.

Language packages share HTTP behavior and fixes while handlers stay in your
language. An HTTP handler can ask CoAkka Runtime to perform named work; each
product can also be used independently.

Outbound requests are part of HTTP Runtime's documented package scope. The
planned standalone **CoAkka HTTP Client** is a separate future product:
[current client scope and roadmap](docs/coakka-http-runtime/explanation/introduction.md#outbound-http-and-the-planned-http-client).

**Package-manager releases are in preparation for npm, Maven Central, Go
modules and PyPI.** The current installation path uses 1.0.0 repository archives.
Start with the [HTTP language guides](docs/coakka-http-runtime/getting-started.md)
and [current archive catalog](coakka-http-runtime/README.md).

### CoAkka HTTP Runtime Inspect

**Swagger-like exploration: browse routes, try HTTP requests and export
OpenAPI 3.0.3.** Inspect has its own browser UI and works with a service that
explicitly enables inspection.

Current Inspect archives cover **macOS ARM64** and **Linux ARM64**. Language APIs
for declaring route/schema metadata are still being completed; check the
integration status below before adding Inspect to an example.

[Walkthrough](coakka-http-runtime/inspect/walkthrough.md) · [Explore Inspect and OpenAPI](docs/coakka-http-runtime/inspect/metadata-and-openapi.md) ·
[Current integration status](docs/coakka-http-runtime/inspect/metadata-and-openapi.md#current-package-integration) ·
[Inspect packages and setup](coakka-http-runtime/inspect/README.md).

## Trying CoAkka for the first time?

Use [Samples](https://github.com/phuong-tran/coakka-samples) to run a small
application in your language. Follow its complete prerequisites, command and
expected result. The normal Runtime connector package supplies its required
Runtime components.

The [package ledger](docs/common/current-packages.md) records exact released
coordinates. [New to CoAkka](docs/common/new-to-coakka.md) explains which product
fits your application.

## Downloading an archive or checking a release?

Choose the version and platform in the product catalog, then follow its
manifest, checksum and platform-evidence instructions. [Product layout](docs/common/product-layout.md)
explains the stable download paths and rolling docs. Each product has its own
version and release; local evaluations are labeled separately.

[All documentation](docs/README.md) ·
[Runtime platform requirements](docs/coakka-runtime/reference/runtime-package-platform-evidence.md) ·
[Artifact contract](docs/common/public-artifact-contract.md) ·
[Support](SUPPORT.md) · [Changelog](CHANGELOG.md)

Contact: gabrielgun1983@gmail.com
