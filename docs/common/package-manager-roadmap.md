# Package Manager Availability

This page lists package-manager coordinates that are currently available for
the public CoAkka artifact surface. Package versions advance independently;
always use the compatibility matrix and the package's release notes to confirm
its native runtime generation.

## Current Packages

| Channel | Runtime | Logger |
| Maven Central | [Runtime 3.0.0](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/runtime/3.0.0), Spring Boot/Quarkus3.0.0 | Logger1.2.2 |
| --- | --- | --- |
| npm | [`coakka-v2-connector-node@3.0.0`](https://www.npmjs.com/package/coakka-v2-connector-node/v/3.0.0) | [`coakka-logger-node@1.2.7`](https://www.npmjs.com/package/coakka-logger-node/v/1.2.7) |
| npm (Bun) | [`coakka-v2-connector-bun@3.0.0`](https://www.npmjs.com/package/coakka-v2-connector-bun/v/3.0.0) | [`coakka-logger-bun@1.2.7`](https://www.npmjs.com/package/coakka-logger-bun/v/1.2.7) |
| npm (Electron) | [`coakka-v2-connector-electron@3.0.0`](https://www.npmjs.com/package/coakka-v2-connector-electron/v/3.0.0) | [`coakka-logger-electron@1.2.7`](https://www.npmjs.com/package/coakka-logger-electron/v/1.2.7) |
| PyPI | [`coakka-v2-connector==3.0.0`](https://pypi.org/project/coakka-v2-connector/3.0.0/) | [`coakka-logger==1.2.2`](https://pypi.org/project/coakka-logger/1.2.2/) |
| Go modules | [`coakka-runtime-go/v3@v3.0.0`](https://github.com/phuong-tran/coakka-runtime-go/releases/tag/v3.0.0) | [`coakka-logger-go@v1.2.6`](https://pkg.go.dev/github.com/phuong-tran/coakka-logger-go@v1.2.6) |
| SwiftPM | [`coakka-runtime-swift` v3.0.0](https://github.com/phuong-tran/coakka-runtime-swift/releases/tag/v3.0.0) | [`coakka-logger-swift` v1.2.2](https://github.com/phuong-tran/coakka-logger-swift/releases/tag/v1.2.2) |
| NuGet | [`CoAkka.Runtime` 3.0.0](https://www.nuget.org/packages/CoAkka.Runtime/3.0.0) | [`CoAkka.Logger` 1.2.3](https://www.nuget.org/packages/CoAkka.Logger/1.2.3) |

JVM, Spring Boot and Quarkus3.0.0 are available from Maven Central. The checked-in Maven layout remains a historical archive. Native archives and source-first connector
packages are indexed by `artifacts/public-artifacts.tsv` and the
[compatibility matrix](../coakka-runtime/reference/compatibility-matrix.md).

Package-lane installation and evidence details live in
[Python 3.0.0](../coakka-runtime/releases/2026-10-30-pypi-python-runtime-3.0.0.md),
[npm Runtime 3.0.0](../coakka-runtime/releases/2026-10-30-npm-node-bun-electron-runtime-3.0.0.md),
[NuGet Runtime 3.0.0](../coakka-runtime/releases/2026-10-30-nuget-csharp-runtime-3.0.0.md) (the earlier package-lane receipt remains historical), and
[SwiftPM Runtime 3.0.0](../coakka-runtime/releases/2026-10-30-swift-runtime-3.0.0-github.md). Go consumers use the
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
