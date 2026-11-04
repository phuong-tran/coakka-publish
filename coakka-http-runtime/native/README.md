# CoAkka HTTP Runtime native SDK

SDK 2.0.1 exposes the focused public C contract:
`<coakka/http/host.h>`, CMake package `CoAkkaHttpHost`, and link target
`CoAkkaHttp::host`. C11, C++20 and language adapters consume the same closed
shared library. Native application business dispatch stays application-owned;
HTTP I/O, route matching, admission, deadlines and authoritative state stay
inside CoAkka HTTP Runtime.

[Download SDK 2.0.1](https://github.com/phuong-tran/coakka-publish/tree/main/coakka-http-runtime/native/releases/2.0.1) and verify the SHA-256 for your target. SDK 2.0.1 implements Core generation 2.0 and C ABI 10.

## Install one matching SDK

Verify and extract the complete archive for the host, then use:

```cmake
find_package(CoAkkaHttpHost 2.0.1 EXACT REQUIRED CONFIG)
add_executable(example main.c)
target_link_libraries(example PRIVATE CoAkkaHttp::host)
```

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/extracted-sdk
cmake --build build --config Release
```

The SDK includes the library, focused public headers, relocatable package
metadata and required license/notice text. Windows includes its import library.
An implementation source tree, diagnostic symbol companion or dependency
inventory is not part of the consumer SDK. Language applications normally
install the corresponding language package instead of selecting native images
independently.

## Lifecycle and borrowing

1. Initialize configuration and routes with their public initializer functions.
   Zero/absent settings select Core defaults. Choose application routes and
   declared bounds; read accepted limits from `service_limits`.
2. On the application dispatcher, enter a Core CPU scope if placement is wanted,
   before creating the service and its children. AUTO is the normal policy.
   It is affinity intent, not a worker count or CPU-time reservation.
3. Create and start the service, checking each result. Query its actual bound
   port, service information, execution observations and CPU facts.
4. Exactly one reader consumes request events. Dispatch using the captured
   handler binding. Copy any data that must outlive a borrowed event, and
   release every successfully taken event exactly once.
5. For an orderly drain, keep readers and handlers progressing until Core's
   drain barrier completes. Stop/join owners, release borrowed results, then
   destroy. Serialize destruction against every other service operation.
6. Restore the dispatcher CPU scope on the entering thread only after all child
   owners retire. Check Core's restoration result.

Destroy is fallible and preserves its owner pointer on refusal. Drain completion
and zero active loops do not prove borrowed memory has been released. Queued
observations and caller-borrowed leases have different disposal laws. Opaque
exchange tokens belong to one service; never construct them from addresses,
slots or a locally guessed generation.

Buffered `respond` and outbound submission copy their supplied bytes within the
synchronous call. A response stream first waits for Core's writable event;
producers do not assume that starting a stream immediately permits a write.
Request-stream events preserve REQUEST before DATA/TRAILERS/END/CANCELLED.
Each installed declaration defines the precise borrowing and refusal law.

Core supplies typed status, issue, effective configuration, route generation,
binding revision and terminal outcome. Never infer them from diagnostic text,
input preferences, elapsed time or application bookkeeping. Operator issue
details are not automatically safe HTTP response bodies.

## Contract migration

The focused host SDK replaces the provisional native application SDK based on
`http.h`, `CoAkkaHttp` and `CoAkkaHttp::runtime`. It does not install that header,
retain aliases, or expose the full internal runtime ABI. Update native callers
together; do not mix the earlier callback SDK with the focused host SDK.
The current focused host contract is ABI10; a product version and an ABI
revision are separate identifiers.

The [public native tests](../runtime-test/README.md) exercise the installed
contract with real loopback I/O, borrowing, pressure, correlation, routing,
streams, concurrency and execution facts. They are reviewable consumer source,
not runtime implementation source. Product qualification separately covers
TLS/mTLS, HTTP/2, HTTP/3, static files, monitoring and runtime instrumentation.

## Targets and evidence policy

| Native image | Qualification lane |
| --- | --- |
| Linux x86_64 `.so` | Linux host plus a separate Windows WSL2 package-execution lane |
| Linux ARM64 `.so` | Physical Raspberry Pi5 |
| Windows x86_64 `.dll` | Physical Intel/AMD Windows |
| Windows ARM64 `.dll` | UTM correctness/compatibility |
| macOS ARM64 `.dylib` | Physical Apple Silicon Mac |

There are five native artifacts. WSL does not introduce a sixth artifact.
Correctness, installation, lifecycle and stability evidence remain distinct
from comparative throughput/latency, which is measured only on physical Pi5.
A cross-build or emulated execution does not replace matching-host evidence.
See the versioned release for the exact archive hashes and qualification scope.
