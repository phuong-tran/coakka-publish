# CoAkka Publish

<p align="center"><img src="docs/common/assets/brand/coakka-logo.png" alt="CoAkka" width="480"></p>

**Shared native cores. Familiar APIs across languages.**

Find packages and documentation for CoAkka's **polyglot, cross-platform
ecosystem**. Its central product, **CoAkka Runtime**, is a **native-backed
distributed runtime for application-owned work** across languages, processes
and hosts.

**CoAkka HTTP Runtime** provides a native HTTP core shared across languages,
and **CoAkka Logger** provides native-backed logging with bounded storage.
Each product can be adopted independently and has its own native core,
packages and version.

Application code can compose the products: receive an HTTP request, call work
owned by a Python process through Runtime, and record events through Logger.
Business rules and the choice of products stay in the application.

This repository is where you find the packages for those products. Choose the
job your application needs, then find the matching language package and
installation guide. Each product can be used independently and has its own
version and distribution channels.

| Product | What it helps you do | Catalog |
| --- | --- | --- |
| CoAkka Runtime | Distributed runtime for application-owned work across processes and languages. | [Runtime packages, tools, addons and evidence](coakka-runtime/README.md) |
| CoAkka HTTP Runtime | HTTP server/client with a native core shared across languages. | [Independent HTTP server/client archives](coakka-http-runtime/README.md) |
| CoAkka Logger | Native-backed logging with bounded storage and visible pressure. | [Logger packages](coakka-logger/README.md) |

Choose a product, then select the exact package coordinate or archive recorded
by its catalog. Verify its manifest, checksum and platform evidence. A packaged
binary does not by itself prove execution on every host.

Existing warehouse directories, Maven endpoints, versioned release paths and
receipts retain their original locations and bytes. Product navigation does
not rename those download URLs. See [Product Layout](docs/common/product-layout.md).
There is no aggregate repository product version or GitHub Release.

Shared guidance:

- [New to CoAkka](docs/common/new-to-coakka.md), [documentation index](docs/README.md) and [ecosystem overview](docs/common/ecosystem-overview.md).
- [Current package coordinates](docs/common/current-packages.md) and [compatibility matrix](docs/coakka-runtime/reference/compatibility-matrix.md).
- [Runtime package/platform evidence](docs/coakka-runtime/reference/runtime-package-platform-evidence.md), [artifact contract](docs/common/public-artifact-contract.md) and [troubleshooting](docs/coakka-runtime/operations/troubleshooting.md).

Private candidate catalogs remain separate from published coordinates. A
source or documentation update does not admit a candidate or publish a release.
Runnable examples live in [CoAkka Samples](https://github.com/phuong-tran/coakka-samples).

[Contributing](CONTRIBUTING.md) · [Support](SUPPORT.md) · [Changelog](CHANGELOG.md)

Contact: `gabrielgun1983@gmail.com`
