# Consuming CoAkka Runtime Android 1.1.0

## Requirements

- Android API 24 or newer;
- `arm64-v8a` device or `x86_64` device/emulator;
- an Android app or started/bound service that owns one runtime lifecycle;
- Kotlin standard library `2.2.10` and protobuf-javalite `4.31.1`, as recorded
  in the release POM.

## Add The Candidate AAR

Copy the AAR into the consuming Android module:

```sh
mkdir -p app/libs
cp path/to/coakka-publish/runtime/android/releases/1.1.0+6516990e/coakka-runtime-android-1.1.0.aar app/libs/
```

Add the exact dependencies to `app/build.gradle.kts`:

```kotlin
dependencies {
    implementation(files("libs/coakka-runtime-android-1.1.0.aar"))
    implementation("org.jetbrains.kotlin:kotlin-stdlib:2.2.10")
    implementation("com.google.protobuf:protobuf-javalite:4.31.1")
}
```

Keep both packaged ABIs during candidate evaluation:

```kotlin
android {
    defaultConfig {
        ndk {
            abiFilters += listOf("arm64-v8a", "x86_64")
        }
    }
}
```

## Embedded Runtime

Use `EMBEDDED` when all runtime work stays in one Android process. It does not
open a TCP listener and does not consume a port:

```kotlin
import coakka.v2.android.AndroidRuntimeConfig
import coakka.v2.android.AndroidRuntimeRoute
import coakka.v2.android.CoAkkaAndroidRuntime
import coakka.v2.android.RuntimeNetworkConfig

val nodeId = "tablet-17"
val runtime = CoAkkaAndroidRuntime.open(
    config = AndroidRuntimeConfig(
        systemName = "factory-floor",
        nodeId = nodeId,
        network = RuntimeNetworkConfig.embedded(),
    ),
    routes = listOf(AndroidRuntimeRoute.local("device.scan", nodeId)),
)
```

The current connector is a low-level host bridge. The app serializes a CoAkka
envelope and submits it with `runtime.submitEnvelope(bytes)`, then consumes the
response or deadletter lane on a blocking I/O worker. Do not invent JVM
connector helper names for this AAR.

## Outbound-Only Runtime

Use `OUTBOUND_ONLY` when the Android app initiates calls to remote runtime
nodes but must not accept inbound TCP connections:

```kotlin
val config = AndroidRuntimeConfig(
    systemName = "factory-floor",
    nodeId = "tablet-17",
    network = RuntimeNetworkConfig.outboundOnly(),
)
```

Remote route endpoints must use their actual reachable host and port. Android
network security policy, VPN, firewall, DNS, and authorization remain app-host
or deployment concerns.

## Network Node

Use `NETWORK_NODE` only when the Android process is supervised and peers must
connect to it:

```kotlin
val config = AndroidRuntimeConfig(
    systemName = "factory-floor",
    nodeId = "tablet-17",
    network = RuntimeNetworkConfig.networkNode(
        bindHost = "0.0.0.0",
        bindPort = 19301,
        advertiseHost = "192.168.1.40",
        advertisePort = 19301,
    ),
)
```

`bindHost` controls the local listener. `advertiseHost` is the address peers
must be able to reach and must never be a wildcard address. Binding to
`127.0.0.1` restricts inbound connections to the device itself.

## Lifecycle And Workers

Own the runtime from `Application`, a started service, or a bound service. Use
a foreground service when the runtime must stay active while the UI is absent,
subject to the target Android version's foreground-service policy.

Use at most one blocking reader for each consumed output lane:

- delivered requests;
- responses;
- deadletters;
- monitor doorbell.

Shutdown ordering is:

1. signal app workers to stop accepting new work;
2. call `runtime.close()` once, which closes the native runtime and adopted
   descriptors;
3. join workers after their blocking reads return.

Do not retain raw file descriptor integers, adopt a descriptor twice, or wait
for a blocked reader before closing the runtime.

## Candidate Verification

Before deploying this candidate, record:

- device model, Android build, API level, and ABI;
- cold start, service restart, UI removal, and process-death behavior;
- embedded request/reply and deadletter behavior;
- outbound reconnect and network-loss behavior when used;
- inbound listener reachability when `NETWORK_NODE` is used;
- resident memory, queue pressure, and long-running lifecycle behavior.
