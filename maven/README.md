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
    implementation("coakka.v2:coakka-jvm-native-runtime-v2:1.2.1-gabde383-fa29f94")
    implementation("coakka.spring:coakka-spring-boot-starter:1.2.1-gfa29f94b59f9")
    implementation("coakka.quarkus:coakka-quarkus-extension:1.2.1-gfa29f94b59f9")
}
```

The logger jar embeds supported native libraries. Consumers should not need a
separate native artifact download for the platforms included in its release
manifest.

Runtime JVM `1.2.1-gabde383-fa29f94` is published against runtime native
package `1.2.1+abde383`. The current Spring Boot and Quarkus Maven artifacts
are `1.2.1-gfa29f94b59f9` and depend on that JVM runtime release.
