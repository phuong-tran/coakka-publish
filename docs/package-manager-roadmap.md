# Package Manager Availability

This page lists package-manager coordinates that are currently available for
the public CoAkka artifact surface. Package versions advance independently;
always use the compatibility matrix and the package's release notes to confirm
its native runtime generation.

## Current Packages

| Channel | Runtime | Logger |
| --- | --- | --- |
| npm | [`coakka-v2-connector-node@2.5.0`](https://www.npmjs.com/package/coakka-v2-connector-node/v/2.5.0) | [`coakka-logger-node@1.2.7`](https://www.npmjs.com/package/coakka-logger-node/v/1.2.7) |
| npm (Bun) | [`coakka-v2-connector-bun@2.5.0`](https://www.npmjs.com/package/coakka-v2-connector-bun/v/2.5.0) | [`coakka-logger-bun@1.2.7`](https://www.npmjs.com/package/coakka-logger-bun/v/1.2.7) |
| npm (Electron) | [`coakka-v2-connector-electron@2.5.0`](https://www.npmjs.com/package/coakka-v2-connector-electron/v/2.5.0) | [`coakka-logger-electron@1.2.7`](https://www.npmjs.com/package/coakka-logger-electron/v/1.2.7) |
| PyPI | [`coakka-v2-connector==2.5.0`](https://pypi.org/project/coakka-v2-connector/2.5.0/) | [`coakka-logger==1.2.2`](https://pypi.org/project/coakka-logger/1.2.2/) |
| Go modules | [`coakka-runtime-go@v1.8.0`](https://pkg.go.dev/github.com/phuong-tran/coakka-runtime-go@v1.8.0) | [`coakka-logger-go@v1.2.6`](https://pkg.go.dev/github.com/phuong-tran/coakka-logger-go@v1.2.6) |
| SwiftPM | [`coakka-runtime-swift` v2.5.0](https://github.com/phuong-tran/coakka-runtime-swift/tree/v2.5.0) | [`coakka-logger-swift` v1.2.2](https://github.com/phuong-tran/coakka-logger-swift/releases/tag/v1.2.2) |
| NuGet | [`CoAkka.Runtime` 2.5.0](https://www.nuget.org/packages/CoAkka.Runtime/2.5.0) | [`CoAkka.Logger` 1.2.3](https://www.nuget.org/packages/CoAkka.Logger/1.2.3) |

JVM, Spring Boot, and Quarkus artifacts are also available through the checked-in
Maven layout in this repository. Native archives and source-first connector
packages are indexed by `artifacts/public-artifacts.tsv` and the
[compatibility matrix](compatibility-matrix.md).

Package-lane installation and evidence details live in
[PyPI](../package-manager/pypi/README.md),
[npm](../package-manager/npm/README.md),
[NuGet](../package-manager/nuget/README.md), and
[SwiftPM](../package-manager/swiftpm/README.md). Go consumers use the
[`coakka-runtime-go` README](https://github.com/phuong-tran/coakka-runtime-go)
and its versioned `CONSUMING.md`.

## Consumption Rules

- Prefer the package manager's normal install command for the language lane.
- Do not assume version numbers match across different package managers.
- Confirm OS, CPU architecture, connector version, and native generation as one
  compatibility tuple.
- Use the release-local manifest and `SHA256SUMS` for downloaded native
  archives.
- Treat bundled bytes, source compilation, and end-to-end execution as separate
  evidence.

Package managers change installation ergonomics only. The language API remains
the application boundary, with the matching native library supplied through
the published package or release artifact.
