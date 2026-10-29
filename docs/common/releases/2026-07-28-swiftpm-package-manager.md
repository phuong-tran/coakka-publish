# SwiftPM Package Manager Release

Swift runtime and logger connectors are now published as public SwiftPM
packages for macOS ARM64.

Current coordinates:

| Surface | Coordinate | Native generation |
| --- | --- | --- |
| Runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `1.3.2` | `1.3.2+caff6d6d` |
| Logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1` | `1.2.1+f50756ebff0d` |

Release evidence:

- `coakka-runtime-swift` public repo exists, `main` at `b19614b4ca82e1b61be9e0622305e37f6657ac7a`, tag `v1.3.2`
- `coakka-logger-swift` public repo exists, `main` at `1ab7e25a5982c4368e15d6a45a7ee36868b78a3b`, tag `v1.2.1`
- clean SwiftPM URL consumer smoke passed for runtime exact `1.3.2`
- clean SwiftPM URL consumer smoke passed for logger exact `1.2.1`
- `coakka-samples` now includes `runtime/swift/basic` and `logger/swift/basic`

SwiftPM distribution does not require an npm/PyPI-style registry account for
this release shape. The public GitHub repository URL plus semantic version tag
is the package coordinate.
