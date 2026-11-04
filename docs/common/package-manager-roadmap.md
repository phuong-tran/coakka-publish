# Package Manager Availability

This page lists package-manager coordinates that are currently available for
the public CoAkka artifact surface. Package versions advance independently;
always use the compatibility matrix and the package's release notes to confirm
its native runtime generation.

## Current Packages

See the [current package ledger](current-packages.md) for all current
registry coordinates and exact installation versions.

JVM, Spring Boot and Quarkus3.0.0 are available from Maven Central. The checked-in Maven layout remains a historical archive. Native archives and source-first connector
packages are indexed by `artifacts/public-artifacts.tsv` and the
[compatibility matrix](../coakka-runtime/reference/compatibility-matrix.md).

Package-lane installation and evidence details live in
[Python 3.2.1](../coakka-runtime/releases/2026-11-05-pypi-python-runtime-3.2.1.md),
[npm Node.js, Bun and Electron 3.2.1](../coakka-runtime/releases/2026-11-05-npm-node-bun-electron-runtime-3.2.1.md),
[Go Runtime 3.2.1](../coakka-runtime/releases/2026-11-05-go-runtime-3.2.1-github.md),
[NuGet Runtime 3.2.1](../coakka-runtime/releases/2026-11-05-nuget-csharp-runtime-3.2.1.md), and
[SwiftPM Runtime 3.2.1](../coakka-runtime/releases/2026-11-05-swift-runtime-3.2.1-github.md). Go consumers use the
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
