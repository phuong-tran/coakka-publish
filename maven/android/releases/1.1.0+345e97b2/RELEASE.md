# Android Connector 1.1.0 Release Evidence

## Identity

- connector version: `1.1.0`;
- native package generation: `2.3.0+345e97b2`;
- native source commit: `345e97b29a95b19d7b24c2e899cd7909db0abd08`;
- minimum Android API: `24`;
- packaged ABIs: `arm64-v8a`, `x86_64`;
- AAR SHA-256: `3ce799885322c9ac92664bf028591bc77432960e7b2d85ecbd3c4e73362bf3cb`;
- source tree recorded by AAR: clean;
- release status: `candidate`.

## Completed Gates

The exact release input passed:

```sh
./gradlew clean testDebugUnitTest assembleRelease sourceReleaseJar \
  generatePomFileForReleasePublication lintRelease
./gradlew publishReleasePublicationToMavenLocal
```

Observed results:

- Gradle build: passed;
- Kotlin unit tests: 3 passed;
- Android lint: 0 errors, 3 dependency/tool version notices;
- AAR contains matching runtime and JNI libraries for both declared ABIs;
- AAR package metadata records the full source commit and clean-tree state
  above, while the release manifest pins the exact AAR digest;
- ELF audit identifies ARM64 and x86-64 payloads correctly; both JNI bridges
  expose the same 10 reviewed JNI entrypoints;
- native dependency audit finds only Android baseline libraries (`libz`,
  `liblog`, `libm`, `libdl`, and `libc`), plus the JNI bridge's expected
  dependency on `libcoakka_runtime_v2.so`;
- Android runtime intake passed for native package generation
  `2.3.0+345e97b2`;
- recursive public-surface scanning passed for the AAR and sources JAR;
- nonblocking pipe I/O handles partial frames, `EAGAIN`, `EINTR`, EOF, and
  cancellation without periodic wakeups;
- explicit network policy no longer derives listener ownership from route
  metadata; focused C regressions pass.

The three lint notices concern newer available Android Gradle Plugin,
compileSdk, and protobuf-javalite versions. The release keeps the reviewed,
pinned toolchain and dependency versions instead of changing them during
intake.

## Missing Matching-Host Evidence

No Android device or emulator was attached during this packaging run. The
following claims are therefore intentionally not made:

- Activity, Service, foreground-service, or process-death lifecycle execution;
- runtime request/reply on Android;
- LAN listener reachability or reconnect behavior;
- long-running memory and queue-pressure behavior on Android hardware.

## Promotion Gate

Promote this AAR into `artifacts/public-artifacts.tsv` and the supported-package
index only after the exact AAR digest passes a named device or emulator smoke
that covers open, route application, start, one terminal request outcome, and
close. The Maven coordinate is an evaluation channel and does not waive this
gate. Record the host image, API level, ABI, command, result, and AAR digest.
