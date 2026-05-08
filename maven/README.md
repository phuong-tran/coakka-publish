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
    implementation("coakka.v2:coakka-jvm-native-runtime-v2:0.1.1-g22f571fd955c")
    implementation("coakka.logger:coakka-jvm-native-logger:0.1.0-gba2a66d98eb5")
    implementation("coakka.spring:coakka-spring-boot-starter:0.1.0-g432bd75d3e4b")
    implementation("coakka.quarkus:coakka-quarkus-extension:0.1.0-g26ee0819dc3d")
}
```

The JVM runtime and logger jars embed supported native libraries. Consumers
should not need a separate native artifact download for the platforms included
in each release manifest.

The Spring Boot starter is a framework adapter. It depends on the shared JVM
runtime artifact and does not embed a Spring-specific native runtime.

The Quarkus extension is a framework adapter. It depends on the shared JVM
runtime artifact and does not embed a Quarkus-specific native runtime.
