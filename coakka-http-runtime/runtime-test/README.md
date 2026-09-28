# CoAkka HTTP Runtime Installed-Package Verification

This suite verifies an installed CoAkka HTTP Runtime SDK through its public C
and C++ interfaces.

## Contents

- [Coverage](#coverage)
- [Build And Run](#build-and-run)
- [Static Analysis](#static-analysis)
- [Evidence Boundary](#evidence-boundary)

## Coverage

| Test | Contract exercised |
| --- | --- |
| `coakka_http_runtime_c_test` | Buffered C server, request/response values, bounds, lifecycle, and real loopback HTTP |
| `coakka_http_runtime_cpp_test` | Direct C++20 consumption of the stable C header and library |
| `coakka_http_runtime_contract_test` | Constants, initializers, invalid inputs, configuration relationships, and bounded rejection |
| `coakka_http_runtime_concurrency_test` | Four client threads, four handler workers, 128 real exchanges, exact success accounting, and complete destruction |
| `coakka_http_runtime_pressure_test` | Bounded admission refusal, recovery, timed stop, retry, and owner retention |
| `coakka_http_runtime_outbound_test` | Logical outbound request, copied inputs, terminal ownership, reader exclusion, interrupt, and drain |
| `coakka_http_core_runtime_test` | `CoAkka HTTP Runtime` request/response and terminal event, health/liveness, monitor config/snapshot/events/policy, stop and release |
| `coakka_http_runtime_protocol_test` | Optional real HTTP/2 TLS and HTTP/3 QUIC/TLS request path using private protocol clients |

Seven tests run by default; the protocol gate adds the eighth. All compile with
strict warnings as errors. C tests use C11; the C++ header consumer uses C++20.
Each CTest case has a 30-second timeout.

## Build And Run

```sh
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The installed SDK must be the complete target package, including its public
C/C++ API and build metadata.

## Static Analysis

Run the consumer sources against the installed public include tree:

```sh
./analyze.sh /path/to/installed/coakka-http-runtime/include
```

The script uses Clang static analysis when available and otherwise runs GCC
`-fanalyzer`, preserving the same strict warning set. Tool absence is an error,
not a passing result.

## Evidence Boundary

This suite proves that the installed CoAkka HTTP Runtime package can be
consumed and that its basic ownership, pressure, concurrency, health, monitoring,
and lifecycle paths execute on the target host. It is correctness evidence, not
a throughput benchmark.

A platform is verified only after its exact artifact also passes architecture,
package-integrity, and matching-host lifecycle checks. Extended source-level
evidence remains part of the private release process.
