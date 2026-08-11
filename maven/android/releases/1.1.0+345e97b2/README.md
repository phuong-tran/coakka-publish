# CoAkka Runtime Android 1.1.0

This directory contains the intake-gated Android connector candidate. It is an
immutable release payload, not a Gradle source workspace.

## Candidate Status

Version `1.1.0` is packaged and host-build verified, but it is not listed in
`artifacts/public-artifacts.tsv`. The evaluation coordinate
`coakka.v2:coakka-runtime-android:1.1.0` resolves from the checked-in Maven
repository, but it must not be presented as a current supported artifact until
the exact AAR passes lifecycle and runtime smoke tests on a named Android
device or emulator image.

## Artifacts

| File | Purpose |
| --- | --- |
| `coakka-runtime-android-1.1.0.aar` | Consumer AAR with Kotlin API, JNI bridge, and native runtime |
| `coakka-runtime-android-1.1.0-sources.jar` | Kotlin and generated Java sources for IDE navigation |
| `coakka-runtime-android-1.1.0.pom` | Maven dependency metadata |
| `coakka-runtime-android-1.1.0.module` | Gradle module metadata |

The AAR contains both `libcoakka_android_jni.so` and
`libcoakka_runtime_v2.so` for:

- `arm64-v8a`, intended for physical ARM64 Android devices;
- `x86_64`, intended for compatible emulators and industrial x86 Android.

Consumers need Android API `36.1` or newer as their compile SDK and may retain
`minSdk = 24` for the packaged runtime floor.

The AAR metadata at `assets/coakka/runtime-package.json` records connector
version `1.1.0`, native package generation `2.3.0+345e97b2`, the full native
source commit, and both packaged ABIs.

## Boundary

The Android layer is deliberately thin. JNI owns lifecycle serialization and
descriptor export; Kotlin adopts each descriptor once and performs framed pipe
I/O. JNI does not retain `JNIEnv`, call Kotlin from native threads, or create a
second event loop.

The default `EMBEDDED` mode creates no TCP listener. `OUTBOUND_ONLY` also has
no listener but permits remote routes. `NETWORK_NODE` requires explicit bind
and advertise addresses and ports.

See [CONSUMING.md](CONSUMING.md) for integration and [RELEASE.md](RELEASE.md)
for exact evidence and promotion blockers.
