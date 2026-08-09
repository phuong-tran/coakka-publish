# SwiftPM Package Lane

SwiftPM is the public source package lane for Swift runtime and logger
connectors.

`coakka-runtime-swift` is the Swift connector in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a Swift-only runtime:
the package uses the same native core and public runtime contract as C++, the
JVM, JavaScript, Python, Go, C#, Rust, and other connector lanes. Kubernetes is
supported but not required.

Current coordinates:

| Surface | Coordinate | Native generation |
| --- | --- | --- |
| Runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `2.1.0` | `2.1.0+60ddf70d` |
| Logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1` | `1.2.1+f50756ebff0d` |

The `2.1.0` runtime source package is prepared from connector source `4782dcd`
over native generation `2.1.0+60ddf70d`. It contains all five native payloads,
the documented file-lane API, and passes Swift build, test, runtime
request/reply, transport, and packaged-consumer smokes on macOS ARM64. The
coordinate is immutable at Git tag `v2.1.0`, commit
`c50948bda185c0488c8c40993b719e7197d7516c`.

The current manifest is [`current.json`](current.json). Public samples consume
these coordinates directly.

Runtime `2.1.0` contains verified native payloads for Linux ARM64/x86-64,
macOS ARM64, and Windows ARM64/x86-64. Swift request/reply, transport, TLS
reload, and consumer execution pass on macOS ARM64. Linux C-bridge compilation
and native payload verification pass; the current Linux guests do not contain
a Swift toolchain. See [package and platform evidence](../../docs/runtime-package-platform-evidence.md).
