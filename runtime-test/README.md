# Auditable Native Evidence Source

This directory is the published source projection of
`coakka-samples/runtime-test/`. It uses only the installed CoAkka
Runtime native C ABI; it does not include private runtime implementation.

In addition to request/reply, bounded-admission, and connection-policy checks,
the projection contains configurable C11 race and route-snapshot hot-reload
evidence. `hot-reload` means full route-snapshot replacement; it does not claim
TLS credential or connection-strategy reload. Producer quotas keep traffic
active through the final generation, and every accepted generation must become
observable in runtime stats before the next traffic slice is released.

The source is intentionally small and separated by responsibility:

- `evidence_config.c`: bounded command-line input parsing;
- `evidence_runtime.c`: request/reply, pressure, stress, and soak workload;
- `file_lane_main.c`: a 9 MiB multi-quantum direct-profile file transfer through
  only the published File Lane contract, including digest, receipt, final
  progress, monotonic observed update cursors, and counter checks;
- `stream_lane_main.c`: 97 ordered variable-size frames through independent
  publisher/subscriber lanes, including metadata, drops, pressure, nonblocking
  waits, counters, and forget lifecycle;
- `connection_strategy_contract.c`: capability-aware connection policy,
  atomic rejection, lifecycle, and post-start immutability checks;
- `concurrency_runtime.c`: multi-producer request/reply, submit-versus-stop,
  independent runtime lifecycle, and atomic route-snapshot replacement;
- `concurrency_config.c` and `concurrency_report.c`: bounded controls and one
  machine-readable concurrency result;
- `evidence_platform.c`: monotonic time, readiness waiting, and duplicate-safe
  host-channel cleanup for Windows, macOS, and Linux;
- `evidence_report.c` and `connection_strategy_report.c`: machine-readable JSON
  output.

## Build

Point CMake at an extracted native runtime package:

```sh
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-runtime-native-v2-<version> \
  -DCOAKKA_NATIVE_EVIDENCE_REQUIRE_CONNECTION_STRATEGY=ON
cmake --build build --config Release
```

Run the workload and connection-policy executables from the selected
configuration directory:

```sh
./build/coakka_runtime_v2_native_evidence smoke
./build/coakka_runtime_v2_connection_strategy_evidence
./build/coakka_runtime_v2_file_lane_evidence
./build/coakka_runtime_v2_stream_lane_evidence
./build/coakka_runtime_v2_concurrency_evidence race --threads 4 --requests 256
./build/coakka_runtime_v2_concurrency_evidence hot-reload \
  --threads 4 --requests 256 --generations 64
```

Set `COAKKA_NATIVE_EVIDENCE_REQUIRE_FILE_LANE=ON` when file-lane evidence is a
required gate. This fails configuration when the selected artifact predates
`coakka/v2/file_lane.h`; otherwise CMake omits only that compatibility target.

Stream Lane is not present in the published `2.1.0` package train. Configure
its evidence target against the exact matching source candidate:

```sh
cmake -S . -B build-stream \
  -DCOAKKA_NATIVE_EVIDENCE_RUNTIME_SOURCE_DIR=/path/to/exact/runtime/source \
  -DCOAKKA_NATIVE_EVIDENCE_REQUIRE_STREAM_LANE=ON
cmake --build build-stream --config Release
./build-stream/coakka_runtime_v2_stream_lane_evidence
```

This target verifies ordered frames, metadata, source-reported drops, pressure,
terminal state, stats, forget, and stop. Do not present it as execution against
a published `2.1.0` package.

For a multi-configuration generator, use `build/Release/` instead.

The current Windows archive contains the published runtime DLL but no MSVC
import library. On MSVC, this harness creates a consumer-only `.lib` from
`coakka_runtime_v2.def` during configure. The DLL is neither modified nor
relinked.

The connection-policy executable covers `PER_EXCHANGE`, `BOUNDED_POOL`,
`PERSISTENT_SINGLE_FLIGHT`, and `MULTIPLEXING`. It evaluates support from the
runtime capability snapshot and requires unsupported modes to fail without
changing active state. It does not load certificates or claim an active
TLS/mTLS handshake.

## Analysis And Sanitizers

Run Clang static analysis against the installed include directory:

```sh
./analyze.sh /path/to/coakka-runtime-native-v2-<version>/include
```

Instrument the consumer harness with AddressSanitizer and
UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build-sanitized \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-runtime-native-v2-<version> \
  -DCOAKKA_NATIVE_EVIDENCE_ENABLE_SANITIZERS=ON \
  -DCOAKKA_NATIVE_EVIDENCE_REQUIRE_CONNECTION_STRATEGY=ON
cmake --build build-sanitized
```

That option instruments this public harness. A sanitizer claim about the
closed runtime binary requires a runtime artifact built with the same
sanitizers. Linux is the authority for leak-detection evidence.

ThreadSanitizer is a separate, mutually exclusive configuration:

```sh
cmake -S . -B build-tsan \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-runtime-native-v2-<version> \
  -DCOAKKA_NATIVE_EVIDENCE_ENABLE_TSAN=ON
cmake --build build-tsan
./build-tsan/coakka_runtime_v2_concurrency_evidence race
./build-tsan/coakka_runtime_v2_concurrency_evidence hot-reload
```

This instruments only the public harness when it links an ordinary production
binary. A Core race claim requires the exact runtime source generation and the
harness to be compiled with TSan in one build graph. The JSON records this
boundary and never treats a release binary as implicitly instrumented.

Sanitizer profiles are intentionally local or evaluator-run. Hosted
`public-surface` CI verifies the artifact contract without paying for
instrumented builds. This keeps the controls auditable without treating a slow
or toolchain-sensitive sanitizer run as a per-change gate.

Run `bash scripts/verify-native-evidence-source.sh` from the repository root
to verify `SOURCE-MANIFEST.sha256`.

Historical prebuilt runners remain under
[`runtime/evidence/native/releases/`](../runtime/evidence/native/releases/).

For loader, unsigned-library, Windows, macOS, Linux, and TLS/mTLS diagnosis,
see [Troubleshooting](../docs/troubleshooting.md).
