# CoAkka HTTP Runtime for native C/C++

Status: ABI 1, GitHub release `1.0.0`.

## Package

The native package contains:

- `include/coakka/http/http.h`, the only public header;
- one closed shared library for the selected operating system and CPU; and
- relocatable CMake metadata exporting `CoAkkaHttp::runtime`.

No runtime source, private header, generated type or language connector is part
of the native package.

Release artifacts are under
[releases/1.0.0+df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431](releases/1.0.0+df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431/).
The suffix identifies the exact native source snapshot; the package version
remains `1.0.0`. The verified targets are macOS arm64, Linux arm64, Linux
x86_64, Windows arm64 and Windows x86_64. Each target passed the same
installed-package tests and exact 21-symbol export check.

## Current Contract

ABI 1 is a buffered HTTP server Request/Response baseline. It provides bounded
server configuration, route registration, Request method/target/header/body
views, one copied Response, typed failures and explicit
create/start/stop/destroy lifecycle.

Request views are valid only during the handler callback. Route configuration
is copied by create. Handler context remains caller-owned until destroy
returns. Response headers and body are copied before a successful response call
returns.

Streaming bodies, WebSocket sessions, static-file delivery, outbound client
requests and TLS are not exposed by ABI 1. Their internal availability is not a
public API promise; each capability must receive its own public contract and
evidence before it can be added.

## CMake

Point CMake at the platform directory and link the imported target:

```cmake
find_package(CoAkkaHttp 1.0.0 EXACT REQUIRED CONFIG)
target_link_libraries(my_server PRIVATE CoAkkaHttp::runtime)
```

```sh
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-http-runtime/native/releases/1.0.0+df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431/<platform>
cmake --build build
```

The [runtime-test](../runtime-test/README.md) directory is the reviewable
black-box consumer for the installed package.

## Distribution Boundary

These are the `1.0.0` GitHub release files. They are not currently mirrored to
a native package registry. The release makes no performance claim; use the
checksums and run the black-box consumer on the deployment target.
