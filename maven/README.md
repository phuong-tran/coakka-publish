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
    implementation("coakka.v2:coakka-jvm-native-runtime-v2:0.2.0-gc124a9e-2bab9ee")
    implementation("coakka.spring:coakka-spring-boot-starter:0.2.0-g5ab812f")
    implementation("coakka.quarkus:coakka-quarkus-extension:0.2.0-g5ab812f")
}
```

The logger jar embeds supported native libraries. Consumers should not need a
separate native artifact download for the platforms included in its release
manifest.

Runtime JVM `0.2.0-gc124a9e-2bab9ee` is published against runtime native
package `0.2.0+c124a9e`. The current Spring Boot and Quarkus Maven artifacts
remain on their `0.2.0-g5ab812f` adapter release line.
