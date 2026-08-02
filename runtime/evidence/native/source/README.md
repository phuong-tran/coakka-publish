# Auditable Native Evidence Source

This directory is the published source projection of
`coakka-samples/runtime/evidence/native/`. It uses only the installed CoAkka
Runtime native C ABI; it does not include private runtime implementation.

The source is intentionally small and separated by responsibility:

- `evidence_config.c`: bounded command-line input parsing;
- `evidence_runtime.c`: request/reply, pressure, stress, and soak workload;
- `connection_strategy_contract.c`: capability-aware connection policy,
  atomic rejection, lifecycle, and post-start immutability checks;
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
```

For a multi-configuration generator, use `build/Release/` instead.

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

Run `bash scripts/verify-native-evidence-source.sh` from the repository root
to verify `SOURCE-MANIFEST.sha256`.

For loader, unsigned-library, Windows, macOS, Linux, and TLS/mTLS diagnosis,
see [Troubleshooting](../../../../docs/troubleshooting.md).
