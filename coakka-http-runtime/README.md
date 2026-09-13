# CoAkka HTTP Runtime 1.0.0

This directory is the public Git-repository distribution for CoAkka HTTP Core
and its Go, Kotlin/JVM, Python, and JavaScript connectors. Core owns listeners,
connections, protocols, files, outbound I/O, deadlines, pressure and shutdown.
Connectors translate language values and application work; they never replace
Core with a host-language HTTP server or client.

## Release

Source identities:

- Core: `coakka-http-runtime@d8deb6b821cdd90b69fa7d8e1c85629aac164315`
- connectors: `coakka-http-runtime-connector@26a28a1c4af3eb65334993532b4eaa082fc75438`

| Lane | Repository artifact | Current release |
| --- | --- | --- |
| [Native C/C++](native/README.md) | five installed SDK trees | `1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315` |
| [Go](go/README.md) | source archive with five embedded Core images | `1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315-26a28a1` |
| [Kotlin/JVM](jvm/README.md) | one five-target JAR | `1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315-26a28a1` |
| [Python](python/README.md) | five platform wheels | `1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315-26a28a1` |
| [JavaScript](javascript/README.md) | one Node/Bun tarball with five prebuilds | `1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315-26a28a1` |

Committed release directories are authoritative. The annotated
`coakka-http-runtime-v1.0.0` tag freezes this artifact snapshot; consumers
should pin that tag or its peeled commit instead of mutable `main`. GitHub
Release pages and duplicate release assets are intentionally not used.

Supported targets are `macos-aarch64`, `linux-aarch64`, `linux-x86_64`,
`windows-aarch64`, and `windows-x86_64`. Every shared Core library statically
contains its non-OS native dependency closure. Linux has a GLIBC 2.28 ceiling;
Windows uses the static MSVC runtime.

The sole public native header is `coakka/http/http.h`. ABI revision 2 exposes
exactly 125 C symbols and 20 capability families. All four connectors project
that complete low-level capability surface. Their buffered `Service` APIs are
smaller convenience layers for ordinary request/reply applications.

## Evidence And Licenses

Every target passed architecture, export, dependency and installed real-HTTP
checks on a matching operating system. Windows x86-64 ran through Windows 11
ARM64's x64 emulation layer, so no physical-x86-64 performance claim is made.
The source-visible [native black-box test](runtime-test/README.md) is included.

Each release directory contains its own manifests, checksums, consumption
instructions and license files. [Third-party notices](THIRD-PARTY-NOTICES.md)
record the exact compiled dependency set. npm, Maven Central, PyPI and a tagged
public Go module remain separate publication gates.
