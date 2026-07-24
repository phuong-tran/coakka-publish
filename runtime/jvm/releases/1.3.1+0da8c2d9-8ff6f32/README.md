# JVM Runtime Connector

`v2/jvm/` is the JVM host connector lane for the standalone CoAkka runtime v2
runtime.

The public packages stay the same:

- `coakka.v2.connector`

The Gradle module prefers the runtime v2 proto directory when a core checkout is
available. CI and source-only connector work fall back to the synchronized
repo-local copies under `v2/proto/`.

The shared Kotlin sources still live under the repo root `src/` tree for now,
but packaging, smoke, publishing, and consumer verification now belong to this
dedicated Gradle subproject so the runtime artifact is not entangled with the
workspace root or the Spring demos.

Current goals:

- keep JNA bindings in sync with the runtime v2 C ABI
- expose Kotlin-friendly and Java-friendly runtime APIs
- keep first-run user code on `CoAkka.local(...)`, `handler(...)`, and
  `ask(...)` before exposing route snapshots or wire envelopes
- keep the first private `runtime-addon-probe` adapter cut process-launch-first
  instead of forcing a premature JNI/JNA probe ABI
- package `v2/jvm` as its own Gradle subproject
- keep embedded-native loading and explicit `-Dcoakka.runtime.lib=...` override
- verify the artifact through a real standalone consumer instead of only
  in-repo project dependencies
- keep the published JVM jar version independent from the bundled native
  package version, with repo-local native staging under `v2/staging/`

Current native packaging note:

- the jar embeds one `libcoakka_runtime_v2` native library per staged platform
- the runtime artifact excludes the in-repo demo package; demos stay source-side
  and must not be published as part of `coakka-jvm-native-runtime-v2`
- current staged coverage is:
  - `macos-aarch64`
  - `linux-aarch64`
  - `linux-x86_64`
  - `windows-aarch64`
  - `windows-x86_64`
- public-ready staged natives must pass the native dependency gate before they
  are embedded into the jar
- consuming the jar must not require a separate native artifact download

Publish repo note:

- runtime JVM release drops should be copied or published under
  `runtime/jvm/releases/` in the artifact repository
- keep this repo responsible for packaging and verification, and keep the
  artifact repository responsible for the released layout

## Build And Test

```sh
./gradlew :v2:jvm:test
```

Focused probe-adapter test:

```sh
./gradlew :v2:jvm:test --tests 'coakka.v2.connector.probe.*'
```

## API Levels

Level 1 is the local text path:

```kotlin
val runtime = CoAkka.local("kotlin-practice")
runtime.handler("hello.en") { name -> "Hello $name" }
val reply = runtime.ask("hello.en", "Nam")
```

Level 2 is explicit runtime control through `ConnectorOrchestrator`,
`RuntimeStartSpec`, `RuntimeRouteSpec`, and route generations.

Level 3 is custom envelope work through `ConnectorEnvelope`,
`submitRawEnvelope(...)`, `submitTypedEnvelope(...)`, delivery hints, and
deadletter observation.

Level 4 is the first private probe-adapter path:

- `RuntimeProbeClient`
- `RuntimeProbeSelection`
- `plan(...)`
- `run(...)`

This Level 4 path is intentionally internal-first.
It shells out to the native probe executable and consumes JSON truth back from
`coakkaCoreNativeDev`.

Current private proof shape:

- fake-process unit coverage for process launch and JSON mapping
- real sibling-core-backed `plan(...)` and `run(...)` smoke against
  `coakka_v2_runtime_addon_probe_ui_alpha`
- current runtime-backed JVM proof cases:
  - `demo.echo / ping`
  - `demo.echo / missing_route_probe`

The JVM lane owns:

- host-friendly selection builder ergonomics
- process launch
- result and trace mapping

The JVM lane does not own:

- probe validation law
- bridge-selection law
- runtime-backed outcome truth
- deadletter reason semantics

Current override knobs:

```text
-Dcoakka.runtime.probe.bin=/abs/path/to/coakka_v2_runtime_addon_probe_ui_alpha
-Dcoakka.core.dir=/abs/path/to/coakkaCoreNativeDev
```

Current default private-path fallback:

```text
../coakkaCoreNativeDev/build-v2/coakka_v2_runtime_addon_probe_ui_alpha
```

That fallback is for sibling-workspace proof only.
It is not a publish-ready probe distribution story.

Current private consumer-smoke path:

```sh
./gradlew :v2:jvm:publishToMavenLocal
./gradlew -p v2/jvm/consumer-smoke runProbeSmoke
```

## Package The Runtime Jar

Host-native resource only:

```sh
./gradlew :v2:jvm:jar
```

Release-shaped jar with versioned host-native plus staged Linux and Windows
runtime natives:

```sh
./gradlew :v2:jvm:packageRuntimeJvmJar
```

Verify that the packaged runtime jar does not contain demo classes or obsolete
native sidecars:

```sh
./gradlew :v2:jvm:verifyRuntimeJvmJarContents
```

Assemble a small distribution folder:

```sh
./gradlew :v2:jvm:distRuntimeJvm
```

Run the packaged jar smoke with embedded native loading:

```sh
./gradlew :v2:jvm:smokePackagedRuntimeJvmJar
```

If the staged Linux natives are missing from repo-local staging, fetch them from
the Linux VMs:

```sh
./gradlew :v2:jvm:fetchLinuxAarch64RuntimeNative
./gradlew :v2:jvm:fetchLinuxX8664RuntimeNative
```

Seed repo-local versioned staging from the current verified natives:

```sh
./gradlew :v2:jvm:stageRuntimeNativeRepoStaging
```

Current release-shaped native coverage:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

## Publish To Maven Local

```sh
./gradlew :v2:jvm:publishToMavenLocal
```

## Runtime Demo And Remote Exchange

```sh
./gradlew :v2:jvm:run
./gradlew :v2:jvm:remoteJvmExchange
```

`remoteJvmExchange` is the local developer proof for the remote runtime path.
It builds a transport-enabled runtime, copies the host runtime library into
`lib/`, starts two JVM processes, and exchanges real remote request/reply traffic
between them. The harness enables
the separate delivered-request lane because remote request delivery should not
share the legacy mixed response lane under concurrent ask traffic.

Do not treat this task as the public release shape yet. The current release jar
still packages the generic `libcoakka_runtime_v2.*` staged natives; remote
publish artifacts need an explicit dependency/runtime-library packaging pass
before samples should claim remote traffic works from the published jar alone.

## Consumer Notes

- [Consuming Guide](CONSUMING.md)
- [Standalone Consumer Smoke](consumer-smoke/README.md)
- [Release Checklist](RELEASE.md)
