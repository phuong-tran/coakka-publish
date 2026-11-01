# CoAkka Publish

<p align="center"><img src="docs/common/assets/brand/coakka-logo.png" alt="CoAkka" width="480"></p>

[Website](https://coakka-ecosystem-preview.vivid-plume-2955.chatgpt.site/) · [Find a package](#choose-your-product) · [Demo](#try-the-nodepython-web-demo) · [Documentation](docs/README.md)

**Shared native cores. Familiar APIs across languages.**

CoAkka is a **polyglot, cross-platform ecosystem** built on shared native cores.
**CoAkka Runtime** is a **native-backed distributed runtime for
application-owned work** across languages, processes and hosts. Your application
keeps its handlers, business rules and data.

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

## Trying CoAkka for the first time?

Use [Samples](https://github.com/phuong-tran/coakka-samples) to run a small
application in your language. Follow its complete prerequisites, command and
expected result. The normal Runtime connector package supplies its required
Runtime components.

The [package ledger](docs/common/current-packages.md) records exact released
coordinates. [New to CoAkka](docs/common/new-to-coakka.md) explains which product
fits your application.

## Downloading an archive or checking a release?

Use the selected product catalog to choose the exact version and platform.
Follow its manifest, checksum and platform-evidence instructions. Local
evaluation packages are separate from published registry coordinates.

Versioned downloads, Maven paths, source mirrors and receipts retain their
original URLs and bytes. [Product layout](docs/common/product-layout.md)
explains the warehouse and rolling documentation. This repository has no
aggregate product version or aggregate GitHub Release.

[All documentation](docs/README.md) ·
[Runtime platform requirements](docs/coakka-runtime/reference/runtime-package-platform-evidence.md) ·
[Artifact contract](docs/common/public-artifact-contract.md) ·
[Support](SUPPORT.md) · [Changelog](CHANGELOG.md)

Contact: gabrielgun1983@gmail.com
