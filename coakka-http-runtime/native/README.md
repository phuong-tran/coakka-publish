# CoAkka HTTP Runtime for native C/C++

Status: ABI 1 development draft. Registry and release publication are not open.

## Package

The native package contains:

- `include/coakka/http/http.h`, the only public header;
- one closed shared library for the selected operating system and CPU; and
- relocatable CMake metadata exporting `CoAkkaHttp::runtime`.

No runtime source, private header, generated type or language connector is part
of the native package.

Current artifacts are under [1.1.0](1.1.0/). The reviewed targets are macOS
arm64, Linux arm64, Linux x86_64, Windows arm64 and Windows x86_64. Each target
passed the same installed-package tests and exact 21-symbol export check.

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
find_package(CoAkkaHttp 1.1.0 EXACT REQUIRED CONFIG)
target_link_libraries(my_server PRIVATE CoAkkaHttp::runtime)
```

```sh
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-http-runtime/native/1.1.0/<platform>
cmake --build build
```

The [runtime-test](../runtime-test/README.md) directory is the reviewable
black-box consumer for the installed package.

## Draft Boundary

These files are published for ABI review and integration testing. They are not
a registry release, semantic-compatibility promise, performance claim or
production-support declaration.
