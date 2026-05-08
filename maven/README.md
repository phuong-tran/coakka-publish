# CoAkka Static Maven Repository

This directory is a public Maven repository layout for JVM artifacts published
by `coakka-publish`.

Gradle example:

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

The logger jar embeds supported native libraries. Consumers should not need a
separate native artifact download for the platforms included in its release
manifest.

Runtime JVM, Spring Boot, and Quarkus Maven artifacts are paused until their
package contents are rebuilt against the sanitized runtime public surface.
