# CoAkka HTTP for Kotlin/JVM

This module provides an idiomatic Kotlin service API and a lower-level typed
runtime API over the host-inlined CoAkka HTTP native library. Java consumers use
the same public classes without Kotlin-only call syntax.

The connector JAR contains managed classes only. It does not embed or extract a
native library. Native package layout and Maven publication are intentionally
outside this development slice.

## Requirements

- Java 8 or newer at runtime;
- JDK 17 to build the Kotlin and native adapter sources;
- an installed CoAkka HTTP host package containing `coakka/http/host.h` and the
  matching shared library;
- the matching native adapter built from this module.

The host library and native adapter must be on the operating system library path.
Tests may instead provide absolute regular-file paths with
`coakka.http.host.path` and `coakka.http.bridge.path`. Both properties are
required together.

## Buffered service

```kotlin
val service = ServiceBuilder()
    .listen("127.0.0.1", 8080)
    .concurrency(2)
    .post("/echo", Handler { request ->
        Responses.bytes(request.bytes(), status = 201)
    })
    .start()

service.use {
    println("listening on ${it.port}")
}
```

`ServiceBuilder` is single-use. `Service` owns one native runtime, one inbound
event reader, a fixed worker pool, and bounded queues. Application handlers do
not run on the native event-loop thread. `close()` is idempotent and preserves
an unfinished native owner if an admitted handler has not stopped by the
configured deadline, so a later close can complete safely.

Header names are indexed case-insensitively for average O(1) lookup. Route and
handler identities are also map-backed; dispatch never scans the route list.

### I/O backend

The default is the platform backend. Linux therefore uses epoll unless the
application explicitly opts in:

```kotlin
val service = ServiceBuilder()
    .ioBackend(IoBackend.IO_URING)
    .get("/health", Handler { Responses.empty() })
    .start()

val info = service.runtimeInfo()
println("requested=${info.ioUringRequested}")
println("effective=${info.ioUringEffective}")
println("fallback=${info.fallbackReason}")
```

The connector only forwards the preference. The native runtime owns support
detection, protocol eligibility, startup, and fallback to epoll. Connector code
does not inspect the operating system or issue a probe syscall. Benchmark code
must check `ioUringEffective` before describing a result as io_uring.

## Supported service features

The same `Service` surface supports:

- HTTP/1.1, HTTP/2, and HTTP/3 listeners with plaintext, TLS, or mutual TLS as
  allowed by the selected protocol;
- buffered and incremental request bodies, including request trailers;
- buffered responses, response streams, response trailers, and SSE;
- WebSocket upgrade, ordered frame callbacks, ping/pong, and bounded close;
- static mounts, SPA fallback, and confined application-selected files;
- logical outbound targets with bounded DNS, pooling, TLS trust, mutual-TLS
  identity, cancellation, and terminal events;
- generation-checked handler rebinding and structural route publication;
- health, active liveness probes, route snapshots, and bounded monitoring.

Unsupported features fail explicitly. The connector does not emulate native
transport behavior.

## Lower-level Kotlin API

Use `httpRuntime {}` when the application needs direct event-lane control:

```kotlin
val runtime = httpRuntime {
    limits(RuntimeLimits(eventLoopThreads = 1))
    listener(Listener(port = 0))
    route(RuntimeRoute(id = 41, method = "POST", path = "/items/{id}"))
}

runtime.start()
try {
    when (val event = runtime.takeEvent(5_000)) {
        is RuntimeEvent.RequestReady ->
            runtime.respond(event.exchange, Responses.text("accepted", 202))
        else -> Unit
    }
} finally {
    runtime.close()
}
```

Java callers use `RuntimeConfiguration` and `createRuntime()` directly. Every
native lease is copied before the adapter call returns. No borrowed pointer crosses
the JVM boundary. Each blocking event lane permits one reader.

## Resource and failure behavior

Construction-time limits are finite. A zero field in `RuntimeLimits` selects a
bounded native default; it does not mean unbounded. The connector additionally
bounds projected headers, event items, byte values, worker queues, retained
stream items, monitor pages, and wait durations.

Native operation failures are reported as `HttpRuntimeException` with a typed
`ResultCode`. Asynchronous handler and stream failures enter the bounded service
error queue and remain available through `pollError()`.

Monitoring is disabled by default. Enabling it reserves fixed storage before
startup. Live policy changes are generation-checked and a rejected change
leaves the effective policy unchanged.

## Build and verification

```bash
./gradlew :connectors:jvm:runtime:check \
  -PcoakkaHttpPrefix=/absolute/path/to/installed-host \
  -PcoakkaHttpBuildDir=/absolute/path/to/external-build
```

`check` compiles the native adapter with warnings as errors, runs the Kotlin and
Java consumer tests on Java 8, exercises the class-only JAR against externally
supplied native libraries, verifies public KDoc, and checks Java 8 bytecode.
On Windows, pass `-PcoakkaHttpPython=C:\\absolute\\path\\to\\python.exe` when
`python.exe` is not already on `PATH`; the same property can select a reviewed
Python interpreter on every platform.

The test runtime defaults to Azul Java 8. Windows ARM64 has no matching Java 8
toolchain in the supported test inventory, so that target uses
`-PcoakkaHttpTestJavaVersion=17` with a native ARM64 JDK. The independent
class-file gate still requires every packaged class to use Java 8 bytecode.

Release verification also shrinks the managed artifact and executes a native
call to prove that only the required runtime lookup names are retained.

The [App Host and connector guide](../docs/app-host-and-connectors.md) describes
ownership and threading. Release evidence remains a gate of the assembled
candidate rather than a promise inferred from this API guide.
