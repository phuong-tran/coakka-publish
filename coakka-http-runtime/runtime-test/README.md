# coakka-http-runtime-test

Read and run the public tests against the HTTP Runtime SDK you install.
The tests check how the SDK handles real requests, refuses invalid operations,
and releases resources when work ends or a service stops.

To review the code, start with [request ownership](host_ownership.c),
[behavior at capacity](host_pressure.c), and the [C++ caller](host_cpp_header.cc).
The [shared test helpers](test_support.c) show how the tests release borrowed
data and handle a failed close.

The suite uses the public C interface from `<coakka/http/host.h>`.
The default is CoAkka HTTP Runtime SDK `2.3.0` with C ABI 10.

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [GitHub Sponsors](https://github.com/sponsors/phuong-tran)

## Build and run

Choose the SDK for your host from the [native SDK catalog](../native/README.md).
Compare the archive hash with its release checksum. Extract the archive,
then run these commands from the root of your `coakka-publish` checkout.

```sh
cmake -S coakka-http-runtime/runtime-test -B build/http-tests \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/extracted-sdk \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/http-tests --config Release --parallel
ctest --test-dir build/http-tests -C Release --output-on-failure
```

CMake requires SDK version 2.3.0 exactly by default. Set
`COAKKA_HTTP_TEST_SDK_VERSION` explicitly to exercise another supported SDK
and use a separate build directory. Record the archive and loaded library
hashes with the results so they identify the build you exercised.
Configure verifies the complete source inventory before compiling. Its SHA256
manifest detects source drift; it is not a publisher signature.

## Installed ABI contract matrix

Use `COAKKA_HTTP_TEST_CONTRACT_MATRIX=ON` for the complete loopback suite in
this checkout. It requires SDK 2.3.0 or later, Python 3.11 or later with
`h2==4.3.0` and `aioquic==1.3.0`, and a curl with both HTTP/2 and HTTP/3.
Provide an absolute path to a fixture certificate, its key and the issuing
CA. The certificate must cover `localhost` and `127.0.0.1`. Use fresh
loopback credentials and a separate build directory for each SDK.

From the root of the checkout, set the SDK and curl paths for your host.
These shell commands are for macOS, Linux and WSL. For native Windows, pass
the same absolute paths to CMake from PowerShell and select the Python
executable from the virtual environment's `Scripts` directory.

```sh
sdk=/absolute/path/to/extracted-sdk
curl_h3=/absolute/path/to/curl
test_source="$PWD/coakka-http-runtime/runtime-test"
test_build="$PWD/build/http-contract"
test_tls="$test_build/tls"
mkdir -p "$test_tls"

"$curl_h3" --version
python3 -m venv "$test_build/peers"
test_python="$test_build/peers/bin/python"
"$test_python" -m pip install 'h2==4.3.0' 'aioquic==1.3.0'
```

Check that the curl output lists both `HTTP2` and `HTTP3` under `Features`.
For a disposable loopback identity, generate a certificate that also serves
as the peer's trust file.

```sh
openssl req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=localhost \
  -addext 'subjectAltName=DNS:localhost,IP:127.0.0.1' \
  -out "$test_tls/server.pem" -keyout "$test_tls/server.key"
cert="$test_tls/server.pem"
key="$test_tls/server.key"
ca="$cert"
```

```sh
cmake -S "$test_source" -B "$test_build" \
  -DCMAKE_PREFIX_PATH="$sdk" -DCOAKKA_HTTP_TEST_SDK_VERSION=2.3.0 \
  -DCOAKKA_HTTP_TEST_CONTRACT_MATRIX=ON -DCOAKKA_HTTP_TEST_CURL="$curl_h3" \
  -DPython3_EXECUTABLE="$test_python" \
  -DCOAKKA_HTTP_TEST_CERTIFICATE="$cert" -DCOAKKA_HTTP_TEST_PRIVATE_KEY="$key" \
  -DCOAKKA_HTTP_TEST_CA_CERTIFICATE="$ca"
cmake --build "$test_build" --config Release
ctest --test-dir "$test_build" -C Release --show-only=json-v1 > test-inventory.json
ctest --test-dir "$test_build" -C Release --no-tests=error --output-on-failure
```

Save the test inventory, CTest output, SDK archive and loaded image hashes,
host and peer versions when reporting results. Fixture instrumentation covers
the consumer process code built with the corresponding sanitizer flags. A
sealed SDK needs its own instrumented build for Core sanitizer evidence.

| Area | Installed consumer cases |
| --- | --- |
| Protocol selection and capacity | `http_host_protocol_*` checks standalone HTTP/1.1, HTTP/2 and HTTP/3 and simultaneous traffic on one shared port. `http_host_multi_listener`, `http_host_h2_admission_reclaim_*` and `http_host_h3_connection_capacity_*` check effective limits, admission and reuse. |
| Requests and uploads | `http_host_runtime`, `http_host_routes`, `http_host_stream`, `http_host_upload_*` and `http_host_lifecycle_*` check request values, body bytes and ownership, multipart interruption, resets and a live sibling. |
| Responses | `http_host_stream_http*`, `http_host_sse_http*`, `http_host_http2_websocket_*` and `http_host_http3_websocket_*` check streaming, trailers and WebSocket on their supported profiles. |
| File delivery | `http_host_files_*` and `http_host_file_gate_*` check HEAD, range, revalidation, file identity and closing during a transfer. A system-call trace is needed to confirm the native sendfile path. |
| Runtime controls | `http_host_routes`, `http_host_tls_reload*`, `http_host_monitor_gate` and `http_host_outbound*` check binding revisions, credential generations, monitor limits, correlation and failure causes. |
| Lifecycle and ABI | `http_host_contract`, `http_host_ownership`, `http_host_concurrency`, `http_host_pressure` and `http_host_cpp` check lease and token ownership, bounded admission, child cleanup and C++ access to the C ABI. |

To check standalone HTTP/1.1 port reuse by itself, configure with
`-DCOAKKA_HTTP_TEST_HTTP1_EPHEMERAL_RELISTEN=ON` and run
`ctest --test-dir "$test_build" -R h1_ephemeral_relisten --output-on-failure`.
The C and C++ consumers start with an automatically chosen port, serve a
request, stop and destroy the listener, then serve a fresh request on the
same port. The cases cover IPv4 and IPv6, plaintext, TLS and mTLS, plus
idle and fixed-port controls and refusal while the first listener is active.
The wire driver uses Python's standard library.

The Linux `*_URING` options add checks that require Core to report effective
`io_uring`. The optional LAN address adds outbound coverage outside loopback.
Per-language installed packages, OS floors, fault injection, extended load,
soak, Inspect, and package metadata still need their own evidence.

### Buffered response capacity

The contract matrix runs `http_host_response_capacity_*` for standalone
HTTP/1.1, HTTP/2 and HTTP/3, and for each protocol on the shared listener.
The C consumer sets a 1,024-byte response limit. A 1,025-byte submission
returns `CAPACITY_EXHAUSTED`, and the same exchange accepts 1,024 bytes.
The wire driver checks the exact body and negotiated HTTP version, then
sends a follow-up request. The consumer checks one terminal per exchange
and a completed drain. Successful response submission leaves request
lease release with the caller.

```sh
ctest --test-dir "$test_build" -R '^http_host_response_capacity_' \
  --no-tests=error --output-on-failure
```

### Slow SSE and gzip fallback

Set `COAKKA_HTTP_TEST_RESPONSE_POLICY=ON` to run just these optional
contract cases with the same installed SDK, Python peers and TLS material.
The full contract matrix enables them as well.

```sh
ctest --test-dir "$test_build" --no-tests=error \
  -R '^http_host_response_policy_' --output-on-failure
```

The fixture holds a slow SSE reader, observes the configured response
retention limit, and checks an independent SSE stream and an ordinary
request while the first remains held. HTTP/1.1 uses separate connections.
HTTP/2 and HTTP/3 use separate streams on the same connection. After the
peer cancels, the fixture matches the terminal events and restarts on the
same port.

The HTTP/1.1 writer deadline case keeps the slow SSE client connected
until Core reports the write timeout and one terminal. It then verifies
that a healthy request still completes and that the fixed port can reopen.

The gzip cases check the raw encoded bytes and exact decompression. They
also exercise explicit gzip refusal, Core selection with the header omitted, a body below
the configured compression threshold, and a 204 with no body. The wire
checks validate the encoding, cache variation and any content length.

### Standalone HTTP/3 WebSocket

Use `COAKKA_HTTP_TEST_HTTP3_WEBSOCKET=ON` to select these checks without the
other matrix groups. Run the following after building.

```sh
ctest --test-dir "$test_build" -C Release --no-tests=error \
  -R '^http_host_http3_websocket_' --output-on-failure
```

The [C fixture](host_http3_websocket.c) owns the listener, exchange tokens
and borrowed events through the public ABI. The
[independent peer](http3_websocket_peer.py) verifies TLS and HTTP/3 and
applies RESET_STREAM, STOP_SENDING and FIN in separate runs. Each run
holds a DATA lease from stream A across its close, then verifies that
stream B echoes and a regular HTTP request completes on that connection.
Other runs verify complete, expired and aborted drain, and stop while an
event reader is waiting. The fixture joins that reader before releasing
the retained DATA lease and destroying the service. Every run checks
that a new service can bind the same UDP port.

## Check the HTTP versions an endpoint serves

Use SDK 2.3.0 for this exercise. The probe also accepts another SDK with
shared-listener support when checking an earlier release.
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
