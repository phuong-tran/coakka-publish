# CoAkka HTTP Runtime for C and C++

**New application?** Start with the [C integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/c/integration.md)
or [C++ integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/cpp/integration.md)
for installation, a first request and feature-by-feature usage. This page
remains the native package reference.

The native application package exposes `<coakka/http/http.h>`. C11 and C++20
applications use the same C contract for ordinary handlers, responses and
explicit service lifecycle. HTTP transport, configuration, resource bounds,
timeouts and operational information remain owned by CoAkka HTTP Runtime.

The October 8, 2026 r3 package set has completed its recorded
five-target checks and is distributed as repository archives. The
older `CoAkka::HttpHost` layout is superseded; do not mix its header or library
with this package. Use the matching artifact-backed consumer samples.

The branch stores the five corrected application archives under
`candidates/2026-10-08-r3/`, with an exact [checksum ledger](candidates/2026-10-08-r3/SHA256SUMS)
and [candidate index](candidate.json). From the publish checkout:

```sh
bash scripts/verify-http-runtime-release.sh --candidate
```

This command verifies only the native candidate bytes and package layout.
Use `--all-candidates` to verify all service and Inspect packages. This command
checks artifacts; it does not create a GitHub Release or publish to registries.

## Contents

- [URL grammar and glossary](../docs/glossary.md)

- [Two distinct packages](#two-distinct-packages)
- [CMake use](#cmake-use)
- [Lifecycle and ownership](#lifecycle-and-ownership)
- [Startup tuning and parsed parameters](#startup-tuning-and-parsed-parameters)
- [Candidate platform evidence](#candidate-platform-evidence)
- [Verification boundary](#verification-boundary)

## Two distinct packages

| Consumer | Header | CMake package | Link target |
| --- | --- | --- | --- |
| C or C++ application | `coakka/http/http.h` | `CoAkkaHttp` | `CoAkkaHttp::runtime` |
| Language connector integration | `coakka/http/host.h` | `CoAkkaHttpHost` | `CoAkkaHttp::host` |

The second package supports language-owned request dispatch. It is not a
replacement for the first package's C application handlers. Both include
`execution.h` and `request_control.h`; keep all three headers with their exact
matching library. Ordinary language applications consume their connector
package, not a separately chosen native library.

## CMake use

After obtaining the verified application package for the target platform,
extract it and point CMake at its installation prefix:

```cmake
find_package(CoAkkaHttp 1.0.0 EXACT REQUIRED CONFIG)

add_executable(example main.c)
target_link_libraries(example PRIVATE CoAkkaHttp::runtime)
```

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/extracted-package
cmake --build build
```

The package supplies the shared library, relocatable CMake metadata, artifact
identity, and required license material. Windows also supplies an import
library. Source trees, build caches, diagnostic binaries, and dependency
inventories are not consumer artifacts.

## Lifecycle and ownership

1. Initialize `coakka_http_server_options_t` with
   `coakka_http_server_options_init`, then declare bounded resources and routes.
2. Create the server with `coakka_http_server_create` and check its result.
3. Start with `coakka_http_server_start`; inspect the actual bound port and
   effective runtime information rather than reconstructing them from inputs.
4. Handle requests using the declared callbacks. Observe the borrowed-view
   lifetimes documented by each operation; retain application callback state
   until server destruction completes.
5. Stop with `coakka_http_server_stop`, check its result, then destroy with
   `coakka_http_server_destroy`. Do not restart a stopped instance.

Check every fallible result. A timeout, pressure refusal, stale request and
closed service are different outcomes; an application must not replace them
with a guessed success or retry automatically. Keep blocking business work
out of HTTP transport processing. Each streaming or asynchronous operation
has its own ownership and completion contract in the installed headers.

The October 7 native correction makes server runtime information read actual
the runtime execution and accepted configuration. A busy lifecycle gate returns
`RETAINED` without blocking; a stopped service returns `CLOSED`. A refused
query leaves the output unchanged. Never treat that previous output as a fresh
observation. Serialize destruction with every query. This correction does not
change the public structure layout and is not a throughput improvement claim.
The current native callback contract is ABI12 with runtime-info version5;
it does change the options/information layouts. Use one complete candidate,
never an older library with the current header.

## Startup tuning and parsed parameters

Initialize `coakka_http_server_tuning_t` with
`coakka_http_server_tuning_init` and optionally assign `options.tuning`.
NULL selects the runtime defaults. The group accepts CPU AUTO/SINGLE, independent
request/terminal notification profiles, and an optional
`coakka_http_compression_t`. Values are borrowed only during create and
copied; there is no hot reload or application loop-count knob.

GZIP transforms eligible buffered responses only. Response streams and SSE
remain uncompressed without being collected into a buffer. When compression
policy is enabled and a client refuses identity coding, the runtime rejects an
unencoded stream with HTTP406 before it starts. Application-supplied content
encoding remains application-owned. Streaming compression is not implemented.

AUTO prefers two allowed logical CPUs on Linux, or one when constrained.
SINGLE requires one. Other platforms currently inherit for AUTO and reject
SINGLE. Runtime-info's grouped `cpu` value reports construction/startup facts;
unknown counts/IDs are not invented observations. Affinity is not a CPU-time
quota or a reservation. The runtime covers construction-time and startup-time workers,
retains the same selected set, and restores each caller thread before returning.
Starting on another thread requires that thread to allow the selected set.

If restoration fails after construction, create returns failure and a non-NULL
destroy-only owner. Applications must destroy that owner and must not start it.
C++ constructors must clean it up before throwing; their destructor does not
run after constructor failure. Failed start remains destroy-only as well.

Notification AUTO resolves inside the runtime; runtime-info returns accepted caps.
GZIP requires a supported provider and explicit level1..9; zero-valued byte
limits select the runtime's bounded defaults. Compression negotiation and framing
remain runtime-owned.

`coakka_http_request_path_parameter_count/path_parameter` and
`coakka_http_request_query_parameter_count/query_parameter` expose indexed,
borrowed runtime-parsed views in callbacks. Each access is O(1), without parsing,
copying or allocating. Query order, duplicates and absent versus empty values
are preserved. Encoded octets remain encoded. Bounds/null refusal returns zero
without changing output; views expire when the callback returns.

## Candidate platform evidence

| Target | Application library | Execution evidence |
| --- | --- | --- |
| macOS ARM64 | `libcoakka_http_runtime.1.0.0.dylib` | Installed consumers on macOS ARM64 |
| Linux ARM64 | `libcoakka_http_runtime.so.1.0.0` | Rocky Linux 8.10 and physical Raspberry Pi 5 on Trixie |
| Linux x86-64 | `libcoakka_http_runtime.so.1.0.0` | Rocky Linux 8.10, emulated x86-64 |
| Windows ARM64 | `coakka_http_runtime.dll` | Windows 11 ARM64 |
| Windows x86-64 | `coakka_http_runtime.dll` | x86-64 process under Windows 11 ARM64 emulation |

Linux packages target glibc 2.28 or newer. The macOS deployment target is 11.0;
this is not a claim of execution on every older macOS version. Windows packages
use the static runtime. macOS x86-64 is not included. Emulation evidence is
functional evidence, not native-hardware performance evidence.

## Verification boundary

Package checks cover exact installed files and exports, architecture, allowed
dynamic dependencies, stripped debug/build paths, legal material, independent
consumers, real loopback HTTP and shutdown. Separate diagnostic builds provide
scoped sanitizer evidence; shipped libraries are not instrumented. Package
checks do not by themselves qualify benchmark claims, signing, registry
publication, or the final synchronized release.
