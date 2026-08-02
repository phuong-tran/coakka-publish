# JVM Runtime Connector

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

The JVM connector brings Kotlin and Java applications into the polyglot,
multi-language, multi-platform CoAkka Runtime ecosystem. Its public package is
`coakka.v2.connector`; the JAR embeds the matching native runtime so consumers
do not need a separate implementation download.

Start with `CoAkka.local(...)`, `handler(...)`, and `ask(...)`. Applications
that own route generations, endpoint inventory, or typed envelopes can use
`ConnectorOrchestrator` and `RuntimeClient` directly.

## Transport Configuration

The connector exposes startup-configured connection strategy, capability
discovery, structured apply results, and atomic TLS/mTLS credential reload.
The public guides document lifecycle, ownership, thread safety, blocking
behavior, capability availability, and Java/Kotlin-facing semantics.

Common ecosystem guidance:

- [CoAkka documentation and samples](https://github.com/phuong-tran/coakka-samples/tree/main/docs)
- [Connection Strategies](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md)
- [TLS And mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
- [Contact And Support](https://github.com/phuong-tran/coakka-publish/blob/main/docs/contact-and-support.md), including `gabrielgun1983@gmail.com`

Native packaging:

- the jar embeds one `libcoakka_runtime_v2` native library per staged platform
- current file coverage is listed below; file presence is not a claim that the
  JVM connector was executed on every platform:
  - `macos-aarch64`
  - `linux-aarch64`
  - `windows-x86_64`
- consuming the jar must not require a separate native artifact download

## Build And Test

```sh
./gradlew :v2:jvm:test
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

Release native file coverage:

- `macos-aarch64`
- `linux-aarch64`
- `windows-x86_64`

Transport-configuration connector evidence:

| Platform/runtime profile | Evidence in this slice |
| --- | --- |
| macOS ARM64 baseline capabilities | JVM ABI/layout, lifecycle, capability, mode, Spring, and Quarkus tests pass |
| macOS ARM64 full capabilities | JVM TLS startup/reload/rejection and advanced-mode tests pass |
| Linux ARM64 | Runtime artifact exists; JVM connector execution is not yet recorded |
| Windows x86-64 on Windows 11 ARM64 compatibility | Runtime artifact exists; JVM connector execution is not yet recorded |

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

## Consumer Notes

- [Consuming Guide](CONSUMING.md)
- [Standalone Consumer Smoke](consumer-smoke/README.md)
- [Release Checklist](RELEASE.md)
