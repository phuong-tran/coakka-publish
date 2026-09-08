# CoAkka HTTP Runtime Test

This directory is the native C11 black-box test surface for CoAkka HTTP
Runtime. It is intentionally separate from the closed runtime implementation
and builds only against one clean installed CoAkka HTTP Core C ABI.

The harness contains no runtime source, private header, private schema,
generated protocol type, language connector, or build-tree fallback. Its
public vocabulary is limited to HTTP routes and the CoAkka HTTP Core contract.

## What It Proves

The executable verifies:

- the exact ABI version and stable status names;
- valid routes, custom HTTP method tokens, encoded path literals, and captures;
- synchronous borrowing: caller method and path storage is neither modified
  nor retained after validation returns;
- invalid pointers, counts, methods, patterns, duplicate IDs, and ambiguous
  routes fail closed with the responsible route index;
- exact route, aggregate segment, capture, and route-declaration byte bounds;
- a failed validation leaves no persistent route state; and
- independent validations remain correct when called concurrently.

It validates only the documented installed C contract. It does not measure
throughput or claim application-server performance. Request and response
behavior remains covered by each language connector's public API tests.

## Build

Point CMake at an isolated installed CoAkka HTTP Core prefix:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-http-core-1.1.0
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Run the executable directly when machine-readable evidence is needed:

```sh
./build/coakka_http_runtime_test > result.json
```

Standard output is exactly one JSON document. Diagnostic failures are written
to standard error. All finite test allocations are owned by the harness and
released before process exit.

## Analysis Builds

Run Clang's static analyzer against the same installed header:

```sh
bash analyze.sh /path/to/coakka-http-core-1.1.0/include
```

Instrument the harness with address and undefined checks:

```sh
cmake -S . -B build-asan-ubsan \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-http-core-1.1.0 \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_ASAN_UBSAN=ON
cmake --build build-asan-ubsan
ctest --test-dir build-asan-ubsan --output-on-failure
```

Thread checks use a separate build:

```sh
cmake -S . -B build-tsan \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-http-core-1.1.0 \
  -DCOAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN=ON
cmake --build build-tsan
ctest --test-dir build-tsan --output-on-failure
```

Harness instrumentation does not instrument an already-built runtime library.
Matching-source runtime sanitizer evidence is a separate release gate.

## Source Integrity

`SOURCE-MANIFEST.sha256` covers every published harness file except the
manifest itself. Verify it before review or execution:

```sh
shasum -a 256 --check SOURCE-MANIFEST.sha256
```

No package-manager release or production-support claim is implied by this
source-visible test surface.
