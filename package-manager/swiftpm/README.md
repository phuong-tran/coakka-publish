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
| Runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `2.1.1` | `2.1.0+60ddf70d` |
| Logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1` | `1.2.1+f50756ebff0d` |

The `2.1.1` runtime source tag keeps the file-lane implementation and all five
native payloads from `2.1.0`, fixes the public file-lane documentation link,
and passes Swift build, test, runtime request/reply, transport, and
packaged-consumer smokes on macOS ARM64. The coordinate is immutable at Git
tag `v2.1.1`, commit `45375fbfc540932a9708d520112efe1346c52d07`.

The current manifest is [`current.json`](current.json). Public samples consume
these coordinates directly.

Bundled runtime generation `2.1.0+60ddf70d` contains verified native payloads
for Linux ARM64/x86-64,
macOS ARM64, and Windows ARM64/x86-64. Swift request/reply, transport, TLS
reload, and consumer execution pass on macOS ARM64. Linux C-bridge compilation
and native payload verification pass; the current Linux guests do not contain
a Swift toolchain. See [package and platform evidence](../../docs/runtime-package-platform-evidence.md).
