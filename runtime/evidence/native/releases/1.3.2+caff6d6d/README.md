# CoAkka Runtime Native Evidence Runner

This release contains prebuilt native evidence runners for the published
CoAkka Runtime native package.

The runner is built from the public sample source:

```text
coakka-samples/runtime/evidence/native/main.c
```

It consumes the published native runtime through the public C ABI and does not
include or reach into private core runtime sources.

## Purpose

This runner provides a local native public-ABI baseline. It is not a claim
about the absolute runtime ceiling, cross-machine performance, connector
performance, production capacity, or an SLO.

Every admitted request follows this complete local path:

```text
caller builds request
  -> native submit path
  -> bounded runtime ingress
  -> route snapshot generation 1
  -> target samples.runtime.native.evidence.local
  -> local echo handler
  -> reply submit
  -> terminal response routing
  -> caller drains response
```

Pressure mode uses the framed request channel to force bounded admission.
Requests that are admitted still complete a reply; rejected requests must
surface as deadletters.

The result is environment-local. Linux, macOS, Windows, CPU model, memory
pressure, thermal state, background workloads, virtualization,
containerization, and power mode can all change the numbers.

## Usage

Prefer the source-first path from `coakka-samples`:

```sh
bash run.sh runtime/evidence/native smoke
```

If the machine does not have a native C toolchain, the sample runner can use
this prebuilt evidence artifact for the current platform when available.

The runner prints one final JSON document. It does not print per-request logs.
Setup and starting messages from the sample wrapper are written to stderr, so
stdout remains suitable for JSON consumers.

Example:

```sh
./bin/coakka-runtime-native-evidence smoke --payload 64K --requests 128
```

Keep the bundled `native/<platform>` directory on the dynamic loader path when
running the binary directly.

The monotonic timing window starts before the first request envelope build and
ends after the final response/deadletter drain. For soak mode, `--duration`
controls submission time; total elapsed time can be longer while admitted work
drains.

The JSON records the target path, terminal counters, source/prebuilt execution
path, compiler, build profile, timing windows, completed local round trips, and
logical request/round-trip payload throughput. Treat those values as evidence
for this exact machine and workload only.
