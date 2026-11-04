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

| Release | Core generation | Go connector | Node.js connector |
| --- | --- | --- | --- |
| First release on Core `2.0` | `2.0` | `2.0.1` | `2.0.1` |
| Go fix | `2.0` | `2.0.2` | `2.0.1` |
| First release on Core `2.1` | `2.1` | `2.1.1` | `2.1.1` |

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
- Native C and C++ applications install the SDK directly and use the
  version recorded in its own release.

The coordinated [2.0.1 release](release-2.0.1.md) uses the SDK packaged as
`2.0.1` for Core generation `2.0` across all five connectors.
