# Consuming `coakka-jvm-native-logger`

## Coordinates

Current Maven coordinates:

- `groupId`: `coakka.logger`
- `artifactId`: `coakka-jvm-native-logger`
- `version`: `0.1.0-gba2a66d98eb5`

Versioning rule:

- the JVM jar version tracks the connector artifact release
- the bundled native package version is recorded separately in the jar metadata
- one published jar version bundles one native package version

## Publish To Maven Local

```sh
./gradlew :logger:jvm:publishToMavenLocal
```

## Dependency Example

```kotlin
repositories {
    mavenCentral()
    maven {
        url = uri("https://raw.githubusercontent.com/phuong-tran/coakka-publish/main/maven")
    }
}

dependencies {
    implementation("coakka.logger:coakka-jvm-native-logger:0.1.0-gba2a66d98eb5")
}
```

## Native Loading

Default path:

- the jar loads the embedded native library for the current platform
- current release-shaped packaging targets `macos-aarch64`, `linux-aarch64`,
  and `linux-x86_64`
- embedded native filenames now carry the staged native package version suffix,
  with an internal fallback to older unsuffixed resource names for compatibility

Override path:

```sh
-Dcoakka.logger.lib=/abs/path/to/libcoakka_logger_core.so
```

or on macOS:

```sh
-Dcoakka.logger.lib=/abs/path/to/libcoakka_logger_core.dylib
```

## Minimal Kotlin Example

```kotlin
import coakka.logger.jvm.JvmLogger

fun main() {
    JvmLogger.start().use { logger ->
        logger.info("orders", """{"status":"ok"}""")
    }
}
```

## Packaged Jar Smoke

```sh
./gradlew :logger:jvm:smokePackagedLoggerJar
```

## Standalone Consumer Smoke

```sh
./gradlew :logger:jvm:publishToMavenLocal
./gradlew -p logger/jvm/consumer-smoke run
```
