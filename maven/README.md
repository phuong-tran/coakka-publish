# CoAkka Static Maven Repository

This directory is a public Maven repository layout for JVM artifacts published
by `coakka-publish`.

## Android Candidate

The complete Android `1.1.0` candidate unit is under
[`android/releases/1.1.0+345e97b2/`](android/releases/1.1.0+345e97b2/README.md).
It contains the AAR, sources JAR, POM, Gradle module metadata, manifest,
checksums, and evidence notes in one Maven-owned lane.

The candidate is available to Gradle as
`coakka.v2:coakka-runtime-android:1.1.0`. This makes evaluation natural for an
Android project and lets Gradle resolve its Kotlin and protobuf dependencies.
It remains a candidate, rather than a supported public artifact, because
matching Android device or emulator lifecycle execution is still a promotion
blocker. Follow its `CONSUMING.md` guide and preserve that evidence boundary.

Gradle example:

```kotlin
repositories {
    mavenCentral()
    maven {
        url = uri("https://raw.githubusercontent.com/phuong-tran/coakka-publish/main/maven")
    }
}

dependencies {
    implementation("coakka.v2:coakka-runtime-android:1.1.0")
    implementation("coakka.logger:coakka-jvm-native-logger:1.2.1-gf50756ebff0d")
    implementation("coakka.v2:coakka-jvm-native-runtime-v2:1.3.3-g282f3ad-657ec79")
    implementation("coakka.spring:coakka-spring-boot-starter:1.3.3-g282f3ad-657ec79")
    implementation("coakka.quarkus:coakka-quarkus-extension:1.3.3-g282f3ad-657ec79")
}
```

The logger jar embeds supported native libraries. Consumers should not need a
separate native artifact download for the platforms included in its release
manifest.

Runtime JVM `1.3.3-g282f3ad-657ec79` is published against runtime native
package `1.3.3+282f3ad`. The current Spring Boot and Quarkus Maven artifacts
are also `1.3.3-g282f3ad-657ec79` and depend on that JVM runtime release.
