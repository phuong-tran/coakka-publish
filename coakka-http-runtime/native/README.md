# CoAkka HTTP Runtime for C and C++

The native package exposes one focused C API in `<coakka/http/host.h>`. C++
applications use the same API. The shared library owns listeners, protocol I/O,
routing, static and application-selected files, outbound calls, deadlines,
monitoring, and orderly shutdown.

The `1.0.0` candidate is not published yet. macOS ARM64, Linux x86-64, Windows
ARM64, and Windows x86-64 have passed their matching-host package gates. Linux
ARM64 still requires its clean Raspberry Pi OS Trixie qualification before the
five-target assembly can be declared ready.

## Installed package

Every target package contains only:

- `include/coakka/http/host.h`;
- the target shared library and required import/link metadata;
- `CoAkkaHttpHost` CMake package files; and
- the legally required `LICENSE` and `NOTICE` material.

Internal build headers, generated schemas, private symbols, tests, debug data,
developer paths, dependency inventories, and language packages are not part of
the installed tree.

## CMake use

Extract the package for the exact operating system and architecture, then point
CMake at that prefix:

```cmake
find_package(CoAkkaHttpHost 1.0.0 EXACT REQUIRED CONFIG)

add_executable(example main.c)
target_link_libraries(example PRIVATE CoAkka::HttpHost)
```

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/coakka-http-host
cmake --build build
```

No private dependency include directory or library belongs in a consumer build.

## Service lifecycle

A service follows one explicit ownership sequence:

1. initialize a `coakka_http_host_configuration_t` and its bounded limits;
2. declare routes, listener security, file authorities, outbound targets, and
   optional monitor reservation;
3. create the service, which copies the complete declaration;
4. start it and read each enabled event lane from exactly one owner thread;
5. answer, fail, stream, or release every admitted event exactly once;
6. begin drain, interrupt readers, stop, and destroy in order.

Request and event views are borrowed until their matching release call.
Successful response and control submissions copy borrowed input. Applications
must keep their own business state and handler scheduling outside the native
event-loop thread.

The runnable [C](https://github.com/phuong-tran/coakka-samples/tree/coakka-http-runtime-samples/coakka-http-runtime/c)
and [C++](https://github.com/phuong-tran/coakka-samples/tree/coakka-http-runtime-samples/coakka-http-runtime/cpp)
samples show the complete startup, request, health, monitoring, drain, and
shutdown sequence.

## Capabilities

The public host API includes:

- HTTP/1.1, HTTP/2, and HTTP/3 listener declarations;
- plaintext, TLS, and mutual TLS configuration;
- buffered and streamed requests and responses, trailers, SSE, and WebSocket;
- immutable static mounts, SPA fallback, and confined file responses;
- logical outbound targets, cancellation, and terminal delivery;
- route snapshots, handler activation, and complete route publication;
- health, fresh liveness, and bounded monitoring; and
- typed failures, explicit pressure, finite deadlines, and finite shutdown.

Availability is checked through the published capability and service-planning
API. A declaration that cannot be supported fails before the service reports
ready; no partial owner or partial route generation is published.

## I/O backend selection

Platform I/O is the default. On Linux this is the normal epoll path. `io_uring`
is used only when the application explicitly requests it and native startup
accepts the selected protocol and operating-system capabilities. If the probe
or initialization cannot be used, startup falls back safely to epoll and
reports requested, probed, effective, and typed fallback state through
`coakka_http_host_service_info`.

Consumers forward the preference. They must not duplicate kernel-version
checks, probe syscalls, or fallback logic.

## Bounds and lookups

Connections, active exchanges, request and completion queues, header and body
bytes, stream slots, files, route-control work, monitor history, and shutdown
waits are finite. Zero-valued optional limits select documented bounded
defaults; they do not mean unbounded.

Route and handler identity dispatch is indexed. Header, query, and path values
remain available in wire/declaration order while language connectors may add
case-normalized or named indexes for average O(1) lookup.

## Monitoring and route changes

Monitoring is disabled by default. Construction reserves the maximum storage
that a later policy may enable. Signals are coalesced hints; applications pull
immutable configuration, health, aggregate, and event snapshots. Monitoring
pressure never blocks HTTP work or changes an exchange result.

Handler activation changes only the selected binding revision. Complete route
publication atomically replaces one immutable structural generation. Rejected
compare-and-apply operations leave the previous effective state unchanged, and
already admitted work finishes through the binding it captured.

## Targets

| Target | Shared library |
| --- | --- |
| macOS ARM64 | `libcoakka_http_host.1.0.0.dylib` |
| Linux ARM64 | `libcoakka_http_host.so.1.0.0` |
| Linux x86-64 | `libcoakka_http_host.so.1.0.0` |
| Windows ARM64 | `coakka_http_host.dll` plus import library |
| Windows x86-64 | `coakka_http_host.dll` plus import library |

Linux requires glibc 2.28 or newer. Windows uses the static MSVC runtime.
macOS x86-64 is not part of this candidate.

## Release verification

Each target gate verifies the exact installed tree rather than a build-tree
substitute. It checks the focused header, public export allow-list, dynamic
dependency allow-list, stripped paths and debug data, package contents, legal
files, C and C++ consumers, service startup, loopback traffic, and shutdown.
Hashes and source identities are recorded by the final assembly verifier.
