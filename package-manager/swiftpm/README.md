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
| Runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `1.3.3` | `1.3.3+282f3ad` |
| Logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1` | `1.2.1+f50756ebff0d` |

The current manifest is [`current.json`](current.json). Public samples consume
these coordinates directly.
