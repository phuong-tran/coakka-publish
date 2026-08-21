# Standalone Consumer Smoke

This project stays outside the root `settings.gradle.kts` on purpose.

It proves that `coakka-jvm-native-runtime-v2` works as a published artifact
through `mavenLocal()` instead of only as an in-repo project dependency.

## Run

First publish the runtime artifact:

```sh
./gradlew :v2:jvm:publishToMavenLocal
```

Then run the standalone consumer:

```sh
./gradlew -p v2/jvm/consumer-smoke run
```

To consume a package dry-run without writing Maven Local:

```sh
./gradlew -p v2/jvm/consumer-smoke run \
  -PcoakkaConsumerMavenUrl=/tmp/coakka-jvm-maven-dryrun
```

Expected output includes:

- `consumer_runtime_info ...`
- `consumer_runtime_smoke ok ...`
- capability discovery, explicit per-exchange startup, and explicit plaintext
  startup succeed through the external artifact

## Private Probe Consumer Smoke

This path stays internal-first.
It proves that the published JVM artifact can launch the sibling native probe
binary and consume probe truth back from `coakkaCoreNativeDev`.

Run:

```sh
./gradlew :v2:jvm:publishToMavenLocal
./gradlew -p v2/jvm/consumer-smoke runProbeSmoke
```

Expected output includes:

- `consumer_probe_smoke ok ...`
