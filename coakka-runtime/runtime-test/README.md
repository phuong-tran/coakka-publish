# Auditable Native Evidence Source

The rolling harness requires a Runtime SDK exposing `coakka_v2_host_handles_close`.
An older SDK is rejected before the harness is built. Historical Runtime
archives remain unchanged; use their matching recorded harness snapshot rather
than pairing them with this current rolling source. Candidate verification
uses the exact SDK identified by the selected candidate catalog.

This directory is the published source projection of
`coakka-samples/coakka-runtime/runtime-test/`. It uses only the installed CoAkka
Runtime native C ABI; it does not include private runtime implementation.

The published/default profile is historical v1. The explicit private Native
candidate profile below uses Core readers/tickets and v2 accounting; it does
not parse responses through the old framed channel.

In addition to request/reply, bounded-admission, and connection-policy checks,
the projection contains configurable C11 race and route-snapshot hot-reload
evidence. `hot-reload` means full route-snapshot replacement; it does not claim
TLS credential or connection-strategy reload. Producer quotas keep traffic
active through the final generation, and every accepted generation must become
observable in runtime stats before the next traffic slice is released.

The source is intentionally small and separated by responsibility:

- `evidence_config.c`: bounded command-line input parsing;
- `evidence_runtime.c`: request/reply, pressure, stress, and soak workload;
- `native_workload.c`, `native_engine_owner.*`, `native_ticket_window.h`: the
  explicit Native profile's public-Core workload and bounded ownership seams;
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

Stream Lane is present in the `2.3.0` native package. Require the header and
target explicitly so an older or incomplete package fails during configuration:

```sh
cmake -S . -B build-stream \
  -DCMAKE_PREFIX_PATH=/path/to/coakka-runtime-native-v2-2.3.0 \
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

In the default historical profile, the connection-policy executable covers `PER_EXCHANGE`, `BOUNDED_POOL`,
`PERSISTENT_SINGLE_FLIGHT`, and `MULTIPLEXING`. It evaluates support from the
runtime capability snapshot and requires unsupported modes to fail without
changing active state. It does not load certificates or claim an active
TLS/mTLS handshake.

## Current-Engine Controls — Source Candidate

The installed-candidate CMake path pins every Core-linked consumer to its exact
selected image, not just the startup probe. Reused install/build RPATH settings
cannot make startup pass while traffic loads an unrelated image. Pure fake-API
and ticket units remain Core-free. CMake also records exact target-file paths
for source launchers, including custom/multi-configuration output layouts.
An installed SDK prefix and a Core source build cannot be selected together.

Samples' macOS/Linux source frontend applies explicit candidate intent to all
workload, concurrency, connection and lane modes, without historical/prebuilt
fallback. This published source projection retains its direct CMake interface;
it does not duplicate Samples' repository-specific shell/bootstrap helpers or
claim Windows Bash execution. Native Windows CMake and exact app-local image
execution are independently checked.

The private native candidate `2.5.1+f6b61768` has focused C11 fixture-owner
controls using only Core's opaque request reader and ask tickets. From the
repository root, select an exact installed `CoAkkaV2` prefix and an external,
caller-owned build directory; no package download or Core rebuild is implied:

```sh
cmake -S runtime-test -B /external/owned/native-engine-controls \
  -DCMAKE_BUILD_TYPE=Release \
  -DCOAKKA_NATIVE_EVIDENCE_RUNTIME_PACKAGE=CoAkkaV2 \
  -DCOAKKA_NATIVE_EVIDENCE_RUNTIME_PREFIX=/exact/installed/native-candidate \
  -DCOAKKA_NATIVE_EVIDENCE_REQUIRE_NATIVE_STARTUP=ON
cmake --build /external/owned/native-engine-controls --config Release \
  --target coakka_runtime_v2_native_engine_owner_tests \
           coakka_runtime_v2_native_engine_owner_consumer \
           coakka_runtime_v2_native_ticket_window_tests \
           coakka_runtime_v2_native_ticket_publication_tests \
           coakka_runtime_v2_native_concurrency_fixture_tests \
           coakka_runtime_v2_connection_strategy_evidence \
           coakka_runtime_v2_concurrency_evidence \
           coakka_runtime_v2_native_evidence
ctest --test-dir /external/owned/native-engine-controls -C Release \
  -R '^native-(engine|ticket|concurrency|connection)' --output-on-failure
```

The white-box control covers exact status/fact projection, partial startup,
stop-refusal retention and cleanup order. The SDK-only consumer verifies16
complete lifecycles, empty/binary roundtrips and Core-issued owned request and
response modes. Release assertions remain active and a disabled-assertion
build is refused. Pre-start snapshots are not live applied-state observations.

This explicit profile selects `native_workload.c` for `smoke`, `pressure`,
`stress` and `soak`; run `coakka_runtime_v2_native_evidence` from the build's
configuration directory. The historical SDK/profile keeps unchanged v1 source,
not a runtime fallback. The same explicit profile now selects Native
`race`/`hot-reload` sources; the historical/default profile retains legacy code.

Native schema `coakka.runtime.native.evidence.v2` separates attempted,
Core-accepted (`submitted`), synchronous `WOULD_BLOCK` without a ticket
(`nativeProfile.synchronouslyRefused`), actual responses (`completed`) and
matched deadletters (`rejected`). It requires:

```text
attempted == submitted + synchronouslyRefused
submitted == completed + rejected
handlerReceived == repliesSubmitted == completed
deadletters == rejected
queueRejected == synchronouslyRefused + rejected + replySubmitBackpressure
```

Pressure is a finite Native admission burst before handler pumping, not the
old pipe-paced scenario; it must prove bounded refusal and complete terminal
conservation. Non-pressure requires no refusal/deadletter/route miss and checks
every reply payload. Original1000ms intent/5000ms final drain remain unchanged.
One FIFO ticket window is allocated once from finite request/max-in-flight
bounds, at most500K slots; unbounded/oversized intent fails before Runtime
construction. Default soak retains at most64 tickets. Core owns reply matching;
the window's4096-round pure test checks ownership/refusal/wrap, not Native state.
Pre-start Core facts are projected only after successful getters, and CLI
refusal never invents a zero-valued runtime fact. No new published artifact or
whole ecosystem release is implied.

The separate pure publication control exercises finite one-producer/controller
ticket ownership,4×4096 release/acquire transfers and no Core load. Native
concurrency uses this handoff plus only Core readers/tickets. Run the selected
`coakka_runtime_v2_concurrency_evidence` with `race` or `hot-reload`; its Native
schema is `coakka.runtime.native.concurrency-evidence.v2`.

Logical attempts, actual synchronous admission backpressure, confirmed Core
admission and matched responses/deadletters remain distinct. The original
5000ms request/default30000ms phase, per-generation quotas, every-generation
Core observation, atomic stale/invalid refusal, final route probes, actual
submit-versus-stop `CLOSED` and lifecycle counts are preserved. No TLS-credential
or connection-policy reload claim is implied.

One finite table defaults to512 pointers/4KiB on64-bit, with an explicit
64×100000-pointer/51.2MB advanced bound excluding Core ticket/result storage.
Controller alone observes/retires tickets; producers join before remaining
owners are freed. Discarded observation never means revoked execution, and an
accepted Core request remains admitted even if fixture handoff fails. Core
RESPONSE remains RESPONSE when the echo payload mismatches; `responseValidation`
keeps that application check separate from the exact Core projection status.
Unknown/unperformed Core counters/states/refusals are null; final state is the
Core stats snapshot before close, not a local lifecycle guess. This is source
candidate proof, not a new published package or complete ecosystem readiness.

The explicit Native profile also selects the connection constructor contract,
emitting `coakka.runtime.native.connection-strategy-evidence.v2`. Run the same
`coakka_runtime_v2_connection_strategy_evidence` executable in that build.
Seven finite cases exercise omitted intent, four explicit modes, explicit pool
tuning and an invalid mode. Core capabilities determine expected availability;
Core validation and construction must agree. Omission passes NULL startup intent
and verifies Core-issued AUTO/MULTIPLEXED defaults, without a connector default
table. Unsupported/invalid construction must refuse with no partial instance.
Only successful getters make accepted startup/delivery/connection facts known;
failed or unperformed facts are null. Snapshots are pre-start accepted config,
not live applied placement.

Native connection intent is immutable from construction: valid old setters
refuse BAD_STATE in both CREATED and STARTED. Invalid setters retain the typed
validation refusal. Atomic apply results and a separate getter must preserve
the actual accepted snapshot, not a guessed default. Legacy retains its old
mutable-before-start contract. Both profiles close exported descriptors through
Core, not the executable's separate CRT on native Windows. This configuration
lane adds no exchange, response pipe parser, certificate or TLS handshake claim.

## Standalone Lane Source Candidate

The current source candidate additionally supports `--owner-aware` on both
File and Stream executables, alongside their no-argument Simple profiles.
Select exact SDK headers with owner-grant APIs; historical prebuilt runners
remain unchanged and are not evidence of this candidate's source execution.
These standalone Core lane APIs do not construct actor Runtime or use its
response engine. Core grants supply the copied exact owner and actual listener
port; File retains bounded resume/idempotency, Stream is single-admission.
Deterministic loopback test tokens are never printed. Both profiles preserve
9MiB+731 File digest/terminal receipts and97 ordered Stream frames, counters,
pressure and stop checks. Unobserved File getter/digest/statistics values are
null on failure, never zero-valued Core facts.

The current Windows import definition is bounded to the harness's public C
call closure; no private export or Core ABI expansion is implied. Both lane
executables receive the exact selected DLL app-local during their actual build.

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
[`runtime/evidence/native/releases/`](../../runtime/evidence/native/releases/).

For loader, unsigned-library, Windows, macOS, Linux, and TLS/mTLS diagnosis,
see [Troubleshooting](../../docs/coakka-runtime/operations/troubleshooting.md).
