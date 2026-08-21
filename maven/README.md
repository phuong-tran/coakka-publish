# CoAkka Static Maven Repository

This directory is a public Maven repository layout for JVM artifacts published
by `coakka-publish`.

## Android

The complete Android `1.1.0` evaluation candidate remains under
[`android/releases/1.1.0+345e97b2/`](android/releases/1.1.0+345e97b2/README.md).
It contains the AAR, sources JAR, POM, Gradle module metadata, manifest,
checksums, and evidence notes in one historical Maven-owned lane.

The current Android `1.2.0` source tag, exact four-ABI AAR, signatures, and API
36 ARM64 release-minified smoke are frozen as internal candidate evidence. No
Maven Central publication is planned and no Android `1.2.0` coordinate is live.
The static repository entry above remains historical evaluation material.

Gradle example:

```kotlin
repositories {
    mavenCentral()
}

dependencies {
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
