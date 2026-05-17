# Consuming `coakka-jvm-native-runtime-v2`

## Coordinates

Current Maven coordinates:

- `groupId`: `coakka.v2`
- `artifactId`: `coakka-jvm-native-runtime-v2`
- `version`: `0.1.1-ge2dc43a-9227dc0`

## Java Baseline

- runtime connector classes are gated to Java 8 bytecode
- the jar declares `Automatic-Module-Name: coakka.v2.runtime` for Java 9+
  module-path users
- Spring Boot 3.x adapters sit above this artifact and require Java 17 because
  Spring Boot 3 requires Java 17

Versioning rule:

- the JVM jar version tracks the connector artifact release
- the bundled native package version is recorded separately in the jar metadata
- one published jar version bundles one native package version

## Publish To Maven Local

```sh
./gradlew :v2:jvm:publishToMavenLocal
```

## Dependency Example

```kotlin
dependencies {
    implementation("coakka.v2:coakka-jvm-native-runtime-v2:0.1.1-ge2dc43a-9227dc0")
}
```

## Native Loading

Default path:

- the jar loads the embedded native library for the current platform
- public-ready runtime jars must include the native runtime for supported
  platforms
- consuming the jar must not require a separate native artifact download or
  extra native implementation packages
- current embedded-native coverage is:
  - `macos-aarch64`
  - `linux-aarch64`
  - `linux-x86_64`

Override path:

```sh
-Dcoakka.runtime.lib=/abs/path/to/libcoakka_runtime_v2.so
```

or on macOS:

```sh
-Dcoakka.runtime.lib=/abs/path/to/libcoakka_runtime_v2.dylib
```

## Minimal Kotlin Example

```kotlin
import coakka.v2.connector.RuntimeClient
import kotlinx.coroutines.runBlocking

fun main() = runBlocking {
    RuntimeClient.startLocal(localTargets = listOf("svc.echo")).use { client ->
        client.registerHandler("svc.echo") { request ->
            RuntimeClient.replyTypedTo(request, source = "svc.echo", payloadUtf8 = request.payloadUtf8())
        }

        val response = client.ask(
            source = "sample-client",
            target = "svc.echo",
            payloadUtf8 = "hello",
        )
        println(response.payloadUtf8())
    }
}
```

## Packaged Jar Smoke

```sh
./gradlew :v2:jvm:smokePackagedRuntimeJvmJar
```

## Standalone Consumer Smoke

```sh
./gradlew :v2:jvm:publishToMavenLocal
./gradlew -p v2/jvm/consumer-smoke run
```
