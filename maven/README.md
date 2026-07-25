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
    implementation("coakka.logger:coakka-jvm-native-logger:1.2.1-gf50756ebff0d")
    implementation("coakka.v2:coakka-jvm-native-runtime-v2:1.3.2-gcaff6d6d-6d5ea58")
    implementation("coakka.spring:coakka-spring-boot-starter:1.3.2-gcaff6d6d-6d5ea58")
    implementation("coakka.quarkus:coakka-quarkus-extension:1.3.2-gcaff6d6d-6d5ea58")
}
```

The logger jar embeds supported native libraries. Consumers should not need a
separate native artifact download for the platforms included in its release
manifest.

Runtime JVM `1.3.2-gcaff6d6d-6d5ea58` is published against runtime native
package `1.3.2+caff6d6d`. The current Spring Boot and Quarkus Maven artifacts
are also `1.3.2-gcaff6d6d-6d5ea58` and depend on that JVM runtime release.
