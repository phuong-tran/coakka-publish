# CoAkka Publish

<p align="center"><img src="docs/common/assets/brand/coakka-logo.png" alt="CoAkka" width="480"></p>

CoAkka is a polyglot runtime ecosystem. This repository is the versioned
artifact warehouse and catalog for independently versioned products.

| Product | Catalog |
| --- | --- |
| CoAkka Runtime | [Runtime packages, tools, addons and evidence](coakka-runtime/README.md) |
| CoAkka HTTP Runtime | [Independent HTTP server/client archives](coakka-http-runtime/README.md) |
| CoAkka Logger | [Logger packages](coakka-logger/README.md) |

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
