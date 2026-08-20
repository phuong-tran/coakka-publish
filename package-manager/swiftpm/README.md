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
| Runtime | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `2.5.0` | `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a` |
| Logger | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.2` | `1.2.1+f50756ebff0d` |

Runtime `2.4.1` and Logger `1.2.2` correct their bundled macOS ARM64 dylibs to
deployment target `13.0` while preserving their native generations, public
API/ABI, and Swift tools baselines. Native verification, package tests, smokes,
and clean consumers cloned from both remote tags pass on macOS ARM64.

The current manifest is [`current.json`](current.json). Public samples consume
these coordinates directly.

Bundled runtime generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a` contains verified native payloads
for Linux ARM64/x86-64,
macOS ARM64, and Windows ARM64/x86-64. Swift request/reply, transport, TLS
reload, and consumer execution pass on macOS ARM64. Linux C-bridge compilation
and native payload verification pass; the current Linux guests do not contain
a Swift toolchain. The Mach-O verifier proves `minos 13.0`; this receipt does
not claim execution on a macOS 13 host. See
[package and platform evidence](../../docs/runtime-package-platform-evidence.md).
