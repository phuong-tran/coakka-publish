# coakka.runtime Maven Central Release

The Maven Central publication is separate from the existing internal/static
publication so current Spring Boot, Quarkus, and staged consumers keep their
coordinates unchanged.

Central coordinates:

```text
io.github.phuong-tran.coakka:runtime:<major.minor.patch>
```

This coordinate represents the product name `coakka.runtime` while following
Maven's group/artifact naming convention. The future logger coordinate is
reserved as `io.github.phuong-tran.coakka:logger`; this Runtime release does not
publish or reserve a logger version.

Published POM metadata links users to:

- [canonical artifacts and documentation](https://github.com/phuong-tran/coakka-publish)
- [runnable samples and learning material](https://github.com/phuong-tran/coakka-samples)

After the Portal reports the selected version as `PUBLISHED`, Gradle consumers
use:

```kotlin
dependencies {
    implementation("io.github.phuong-tran.coakka:runtime:2.5.0")
}
```

Maven consumers use:

```xml
<dependency>
  <groupId>io.github.phuong-tran.coakka</groupId>
  <artifactId>runtime</artifactId>
  <version>2.5.0</version>
</dependency>
```

The Central artifact supports Java 8 and newer JVMs. It does not create a
separate artifact for every JVM release. Optional newer-JVM implementations may
be added behind the same API only after workload and compatibility evidence
justify them.

The `2.5.0` Central packaging release embeds exact native generation
`2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. Consequently,
`RuntimeInfo.runtimeVersion` reports `2.5.0`;
the Maven version identifies the immutable JVM distribution, while runtime
identity continues to identify the native engine generation.

## Public Surface

Uploading the bundle publishes the runtime jar, Kotlin sources, generated
Javadoc, dependency metadata, native runtime binaries, and their signatures and
checksums. `LICENSE.md` is an exact package projection of the governing CoAkka
Public Artifact License 1.1 from the canonical `coakka-publish` public docs.

Do not upload until all of these external gates are closed:

- `io.github.phuong-tran` is visible as a verified namespace in the Central
  Publisher Portal; the `io.github.phuong-tran.coakka` subgroup is published
  beneath that verified namespace
- the clean release version is final and has never been published
- the source commit and exact native generation are frozen and tagged
- the production PGP public key is distributed to a supported keyserver
- matching-host package/runtime evidence passes for every advertised native
  platform
- a Publisher Portal user token is available only in the release environment

Central releases are immutable. A rejected or incorrect version must be fixed
under a new version, never overwritten.

## Signing Inputs

Use an ASCII-armored private PGP key through environment variables. Do not put
keys, passphrases, or Portal tokens in `gradle.properties` inside this repo.

```sh
export COAKKA_MAVEN_SIGNING_KEY="$(cat /secure/path/coakka-central-secret.asc)"
export COAKKA_MAVEN_SIGNING_PASSWORD='...'
```

The public key must be uploaded separately. Gradle exposes
`uploadPublicPgpKey` for this purpose, or an established GPG workflow may be
used. Keep the private key outside the repository and backed up securely.

## Build And Verify

Choose the final clean version. The current candidate is `2.5.0`; replace it if
the release train advances before upload.

```sh
./gradlew :v2:jvm:bundleRuntimeJvmForMavenCentral \
  -PcoakkaV2JvmVersion=2.5.0 \
  -PcoakkaMavenCentralNamespaceVerified=true
```

The build fails closed for a dirty worktree, a non-clean version, an
unacknowledged namespace, a missing signing key, license drift, incomplete POM
metadata, missing artifacts, missing ASCII-armored signatures, bad checksums,
empty sources/Javadoc, or an invalid package shape.

Expected output:

```text
v2/jvm/build/central/runtime-2.5.0-central-bundle.zip
```

Run the external Java 8 consumer against the exact staged Central publication:

```sh
./gradlew :v2:jvm:verifyRuntimeJvmCentralConsumerJava8 \
  -PcoakkaV2JvmVersion=2.5.0 \
  -PcoakkaMavenCentralNamespaceVerified=true
```

Before upload, independently verify all five detached signatures with the
production public key:

```sh
artifact_dir=v2/jvm/build/central/bundle-root/io/github/phuong-tran/coakka/runtime/2.5.0
for signature in "$artifact_dir"/*.asc; do
  gpg --verify "$signature" "${signature%.asc}"
done
```

## Upload

Generate a Central Publisher Portal user token and expose its base64 bearer
value only to the release shell:

```sh
export CENTRAL_PUBLISHER_TOKEN='...'
scripts/upload_maven_central_bundle.sh \
  v2/jvm/build/central/runtime-2.5.0-central-bundle.zip \
  coakka.runtime-2.5.0
```

The script always creates a `USER_MANAGED` deployment. Review Central's
validation result in the Publisher Portal before manually publishing. Do not
switch the first release to automatic publishing.

After publication, resolve the immutable Central coordinate from a clean Maven
cache on Java 8 and a current supported JVM, and archive the Portal deployment
ID, source commit, native generation, key fingerprint, bundle SHA-256, and test
evidence with the release record.
