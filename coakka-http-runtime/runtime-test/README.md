# CoAkka HTTP Runtime Native Test

This is a black-box consumer of the closed CoAkka HTTP Runtime native package.
It includes only `coakka/http/http.h`, links the installed shared library, and
does not read runtime source or private headers.

The four executables cover every public ABI 1 function:

- constants, result names, structure initializers and C++20 header consumption;
- invalid pointers, structure sizes, byte views, route declarations, duplicate
  route identities, configuration relationships and aggregate-size overflow;
- create, start, bound-port, repeated stop, destroy and invalid lifecycle
  transitions;
- real loopback Request method, scheme, authority, target, headers and body;
- invalid Response rejection without consuming the valid terminal operation;
- copied Response header/body ownership, response-size rejection with recovery,
  duplicate terminal rejection and route-miss behavior; and
- concurrent real requests through independently scheduled client and handler
  workers, followed by complete server destruction.

The default concurrency run uses four client threads, 32 requests per client
and four handler workers. Its arguments are bounded to 32 clients, 10,000
requests per client, 32 handler workers, 1,000 complete lifecycle cycles and
1,000,000 total requests. Invalid or excessive input is rejected before any
server or client resource is created.

## Build And Run

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Run a deeper bounded concurrency and lifecycle campaign directly:

```sh
./build/coakka_http_runtime_concurrency_test \
  --clients 8 \
  --requests-per-client 256 \
  --server-workers 4 \
  --cycles 20
```

The C runtime, contract and concurrency executables emit one JSON object on
success. This is correctness and lifecycle evidence, not a performance
measurement.

## Analysis And Sanitizers

Run Clang static analysis against the installed include directory:

```sh
./analyze.sh /path/to/installed/coakka-http-runtime/include
```

Instrument the public test harness with AddressSanitizer and
UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build-sanitized \
  -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_SANITIZERS=ON
cmake --build build-sanitized
ctest --test-dir build-sanitized --output-on-failure
```

ThreadSanitizer requires a separate build:

```sh
cmake -S . -B build-tsan \
  -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN=ON
cmake --build build-tsan
ctest --test-dir build-tsan --output-on-failure
```

These options instrument this public consumer. A finding-free run does not
claim that an ordinary closed release library was itself sanitizer-instrumented.

This test surface accompanies GitHub release `1.0.0`. A platform is verified
only after its own artifact has passed this black-box test and binary inspection
on that operating system and architecture.
