# CoAkka HTTP Runtime for Native C/C++

Current GitHub release:
[`1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315`](releases/1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315/README.md).

The release contains five installed SDK trees. Each has the sole public header
`coakka/http/http.h`, relocatable CMake metadata and one dependency-closed Core
shared library. Windows also carries the matching import library.

Use the [source-visible black-box consumer](../runtime-test/README.md) to verify
an installed tree without access to private runtime headers or schemas.
