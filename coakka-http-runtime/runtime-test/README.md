# coakka-http-runtime-test

Read and run the public tests against the HTTP Runtime SDK you install.
The tests check how the SDK handles real requests, refuses invalid operations,
and releases resources when work ends or a service stops.

To review the code, start with [request ownership](host_ownership.c),
[behavior at capacity](host_pressure.c), and the [C++ caller](host_cpp_header.cc).
The [shared test helpers](test_support.c) show how the tests release borrowed
data and handle a failed close.

The suite uses the public C interface from `<coakka/http/host.h>`.
The default SDK is `2.0.1`, with C ABI 10.

## Build and run

For the macOS ARM64 SDK download and toolchain setup, see the [tested build steps](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/inspect/walkthrough.md#build-and-test-against-the-native-sdk).

From the root of your `coakka-publish` checkout, verify and extract the
[2.0.1 SDK for your host](../native/releases/2.0.1/), then run:

```sh
cmake -S coakka-http-runtime/runtime-test -B build/http-tests \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/extracted-sdk \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/http-tests --config Release --parallel
ctest --test-dir build/http-tests -C Release --output-on-failure
```

CMake requires SDK version 2.0.1 exactly by default. The explicit
`COAKKA_HTTP_TEST_SDK_VERSION` cache entry permits qualification of a different
complete SDK; such a run is evidence for that SDK, not evidence for 2.0.1.
Configure verifies the complete source inventory before compiling. Its SHA256
manifest detects source drift; it is not a publisher signature.

## Check the HTTP versions an endpoint serves

Use [SDK 2.2.0](https://github.com/phuong-tran/coakka-http-runtime-go/releases/tag/v2.2.1) or another SDK that supports listener V1.
Install Python 3.8 or later and choose a curl whose
`--version` lists both HTTP2 and HTTP3. In the configure command above, set
`-DCOAKKA_HTTP_TEST_SDK_VERSION` to the version of your extracted SDK and add
`-DCOAKKA_HTTP_TEST_CURL=/absolute/path/to/curl`. Build, then run
`ctest --test-dir build/http-tests -R '^http_host_protocol_' -V`.

The four tests start temporary TLS endpoints on loopback. Three run
HTTP/1.1, HTTP/2 and HTTP/3 individually. The mixed test sends all three
versions concurrently to one V1 port. Its JSON report includes the endpoint,
the versions Core reports as accepting requests, and the versions the clients
actually used. Each request must return HTTP 200 with the expected body and
negotiate the requested version.

## What the tests prove

| Case | Invariant |
| --- | --- |
| runtime | Real loopback request, parsed path/query, synchronous input copies and exact response bytes |
| contract | Accepted Core settings, invalid output preservation, empty-read timeout, stop and idempotent disposal |
| ownership | Cross-service and retired tokens refused; drain/stop do not discharge a live borrowed lease |
| pressure | One retained admission, bounded refusal, invalid response does not consume reply ownership, recovery |
| outbound | Logical selection, copied intent, response correlation and cancellation terminal |
| outbound_address | A hostname without a configured resolver is rejected before any request bytes are sent. The terminal event belongs to the submitted call and appears once. |
| tls_reload | Credential apply and query, TLS and mTLS trust rotation over loopback, admitted responses survive drain, typed refusal and stop, monitor remains off |
| routes | Coherent snapshot, captured old binding, revision conflict preserves state, exact activation replay |
| stream | Ordered request chunks/trailers/end, Core writable notification, bounded response stream and trailers |
| concurrency | Four clients, one event reader, 32 distinct replies and proven worker joins |
| execution | Absent/explicit/malformed construction intent, Core CPU facts, configured versus active loops |
| cpp | Standard-layout C records and checked C++ create/inspect/destroy |
| fixture_wire | Truncation, framing conflict, overflow and wrong-body/trailer rejection in the fixture assertion |

For the optional LAN regression, configure with
`-DCOAKKA_HTTP_TEST_OUTBOUND_LAN_ADDRESS=<local-ipv4>`. Use a non-loopback
IPv4 address assigned to the machine running the suite. This adds
`http_host_outbound_lan`, which sends a request to a local fixture via that IP.

The TLS case uses two disposable loopback-only identities and the public
outbound API. It starts a fresh outbound owner for each handshake so a pooled
connection cannot hide a trust change. The fixture certificates expire on
2045-01-01 UTC. This case is required for SDK 1.1.2 and later.

POSIX hosts run an additional white-box fixture test: refusal at each partial
thread-creation position must release and join every child actually created.
This helper test is not a separate runtime capability.

The socket helper owns nonblocking loopback descriptors, one monotonic 10-second
budget per operation, partial I/O and interruption handling, and a 16KiB output
ceiling. These are fixture limits, not duplicated runtime defaults. The thread
helper caps groups at32, publishes immutable inputs at creation, and reads
results only after joins. Unproven coordination/join aborts the fixture process
before freeing a possibly live child context. CTest supplies a 120-second
process watchdog; a portable join is not a hard realtime guarantee.

Exactly one reader owns each service's request-event lease. Bytes borrowed from
an event expire at release. Opaque exchange tokens are returned to their owning
service and never reconstructed. The application retains business-handler
ownership. A failed destroy keeps its pointer; successful drain is not proof
that borrowed memory can be freed. Controller CPU scope entry/restore uses
Core-issued results on the same thread, after all child owners retire.

## Instrumented fixture checks

Use separate build directories for `address`, `undefined`, and `thread`.
For example, add these flags to a supported Clang/GCC configuration:

```sh
-DCMAKE_C_FLAGS="-fsanitize=undefined -fno-omit-frame-pointer"
-DCMAKE_CXX_FLAGS="-fsanitize=undefined -fno-omit-frame-pointer"
-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=undefined
```

Run with `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. For address use
`ASAN_OPTIONS=halt_on_error=1:detect_leaks=1`; for thread use
`TSAN_OPTIONS=halt_on_error=1`. These instrument the consumer, not a prebuilt
production library. A tool that fails before execution is blocked evidence.
`analyze.sh /absolute/sdk/include` analyzes only the owned C fixture source.

Clang users may enable `COAKKA_HTTP_TEST_FUZZ=ON` and run
`http_fixture_fuzz -max_total_time=30 -max_len=16384` with a task-owned corpus
and artifact directory. This targets the fixture's narrow wire assertion; it
is not product parser fuzz evidence.

## What a pass tells you

These tests check the behaviors listed above through the installed package.
They are one part of release verification. They do not measure performance
or establish that every use of the SDK is safe.

Run `bash coakka-http-runtime/scripts/verify-http-runtime-test-source.sh` from
the repository root to check that the test files match their recorded checksums.
