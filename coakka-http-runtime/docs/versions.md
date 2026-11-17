# Versions and SDK compatibility

Check the release notes for your connector version before choosing a native
SDK. They identify the SDK version tested with that connector. Each release
also records the SHA-256 checksums for the native files it uses.

## How we assign versions

A Core generation has two numbers, such as `2.0`. A new Core generation
changes one or both of those numbers. Each generation identifies a
qualified Core SDK and its checksums.

A connector adds a third number. It starts at `1` when that connector
first uses a Core generation. A fix in one connector advances only its
third number and keeps the exact Core SDK pin.

For example, in connector `2.3.1`, read `2.3` as the Core generation
and the final `1` as the first revision of that connector on Core 2.3.
Its exact native SDK is `2.3.0` with C ABI 10. If only Go changed next,
Go could move to `2.3.2` using that same SDK and the other connectors
could remain on `2.3.1`.

| Release | Core generation | Go connector | Node.js connector |
| --- | --- | --- | --- |
| First release on Core `2.0` | `2.0` | `2.0.1` | `2.0.1` |
| Go fix | `2.0` | `2.0.2` | `2.0.1` |
| First release on Core `2.1` | `2.1` | `2.1.1` | `2.1.1` |
| Coordinated release on Core `2.2` | `2.2` | `2.2.1` | `2.2.1` |
| Coordinated release on Core `2.3` | `2.3` | `2.3.1` | `2.3.1` |

Published package versions and checksums stay fixed. Documentation on
`coakka-publish` and `coakka-samples` can be corrected independently.

## What to install

- Node.js, Bun, and Python install the native files packaged with the
  connector. Install the connector version for your application.
- Java and Kotlin use a managed JAR and a native ZIP classifier. Select
  the classifier for your JVM and use the JAR and ZIP from the same
  connector version.
- Go uses the tagged Go module and a separately installed native SDK.
  Download the SDK version named in the Go release notes for that tag.
- C# installs the NuGet package for its target. Swift resolves the SwiftPM
  package at the exact connector tag. Both include the matching native files.
- Native C and C++ applications install the SDK directly and use the
  version recorded in its own release.

The coordinated [2.0.1 release](release-2.0.1.md) uses the SDK packaged as
`2.0.1` for Core generation `2.0` across all five connectors.

The coordinated [2.2.1 release](release-2.2.1.md) pins SDK `2.2.0` for
Core generation `2.2`. The C ABI is 10.
