# CoAkka HTTP Runtime Native Test

This is the source-visible black-box test for the CoAkka HTTP Runtime native
connector. It includes only `coakka/http/http.h`, links the installed shared
library, and does not read runtime source or private headers. The test therefore
exercises the same Request, Response, configuration, and lifecycle surface that
a native C or C++ consumer receives.

The four executables cover every public ABI 1 function:

- constants, result names, structure initializers and C++20 header consumption;
- invalid pointers, structure sizes, byte views, route declarations, duplicate
  route identities, configuration relationships and aggregate-size overflow;
- create, start, bound-port, repeated stop, destroy and invalid lifecycle
  transitions, including 128 stop-before-start teardown cycles in one process;
- real loopback Request method, scheme, authority, target, headers and body;
- invalid Response rejection without consuming the valid terminal operation;
- copied Response header/body ownership, response-size rejection with recovery,
  duplicate terminal rejection and route-miss behavior; and
- concurrent real requests through independently scheduled client and handler
  workers, followed by complete server destruction;
- stop while client calls are active, with exact completed, closed, and
  unexpected-failure accounting; and
- simultaneous independent create, start, port, repeated stop, and destroy
  lifecycles with no server ownership shared between test threads.

Concurrency profiles are explicit and bounded:

| Profile | Purpose | Default workload |
| --- | --- | --- |
| `smoke` | Routine concurrent Request/Response lifecycle | 4 clients x 32 requests x 1 cycle |
| `stress` | Repeated high-contention request batches and full destruction | 8 clients x 256 requests x 4 cycles |
| `race` | Active-client stop plus independent concurrent server lifecycles | 4 clients x 64 requests, then 4 x 8 lifecycles |
| `soak` | Longer evaluator-run lifecycle campaign | 8 clients x 512 requests x 20 cycles |

Arguments are bounded to 32 clients, 10,000 requests per client, 32 handler
workers, 1,000 cycles, 1,000 lifecycle iterations per client, 1,000,000 total
requests, 10,000 total lifecycles, and a 60-second operation deadline. Invalid
or excessive input is rejected before any server or client resource is
created. Deadlines use a monotonic clock, and all created threads and server
owners are joined or destroyed before the test returns.

## Build And Run

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Run a deeper bounded concurrency and lifecycle campaign directly:

```sh
./build/coakka_http_runtime_concurrency_test \
  soak \
  --clients 8 \
  --requests-per-client 256 \
  --server-workers 4 \
  --cycles 20 \
  --timeout-ms 60000
```

The C runtime, contract and concurrency executables emit one JSON object on
success. This is correctness and lifecycle evidence, not a performance
measurement.

## Analysis And Sanitizers

Run Clang static analysis against the installed include directory:

```sh
./analyze.sh /path/to/installed/coakka-http-runtime/include
```

AddressSanitizer, UndefinedBehaviorSanitizer, and ThreadSanitizer use separate
build directories so each result has an unambiguous configuration.

AddressSanitizer:

```sh
cmake -S . -B build-asan \
  -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_ASAN=ON
cmake --build build-asan
ASAN_OPTIONS=halt_on_error=1:detect_leaks=1 \
  ctest --test-dir build-asan --output-on-failure
```

UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build-ubsan \
  -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_UBSAN=ON
cmake --build build-ubsan
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  ctest --test-dir build-ubsan --output-on-failure
```

ThreadSanitizer:

```sh
cmake -S . -B build-tsan \
  -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN=ON
cmake --build build-tsan
TSAN_OPTIONS=halt_on_error=1:ignore_noninstrumented_modules=1 \
  ctest --test-dir build-tsan --output-on-failure
```

These options instrument this public consumer, including its thread gates,
socket ownership, counters, and lifecycle coordination. The thread profile
ignores observations originating only inside the ordinary release library; its
result therefore applies to the consumer harness, not to the library's internal
concurrency. Internal race evidence requires a separately instrumented library
build. The hosted Linux jobs run all three consumer configurations;
matching-host execution remains required for each advertised artifact.

This test surface accompanies GitHub release `1.0.0`. A platform is verified
only after its own artifact has passed this black-box test and binary inspection
on that operating system and architecture.
