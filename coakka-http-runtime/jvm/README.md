# CoAkka HTTP For Java And Kotlin

One JVM package serves Java and Kotlin. It provides a buffered service builder
for ordinary handlers and a complete typed `CoAkka HTTP Runtime` surface
for protocols, streaming, realtime traffic, security, live control, and
monitoring.

## Contents

- [Install](#install)
- [Java Quick Start](#java-quick-start)
- [Kotlin Quick Start](#kotlin-quick-start)
- [Two Useful API Levels](#two-useful-api-levels)
- [Capacity And Backpressure](#capacity-and-backpressure)
- [Observability And Monitoring](#observability-and-monitoring)
- [TLS, mTLS, And Live Handler Changes](#tls-mtls-and-live-handler-changes)
- [Lifecycle](#lifecycle)
- [Targets And Current Gate](#targets-and-current-gate)

## Install

The current JAR remains private. Place the exact staged artifact under `libs/`:

```kotlin
dependencies {
    implementation(files("libs/coakka-http-jvm-1.0.0.jar"))
}
```

The JAR selects and verifies the matching target image at runtime. No separate
native runtime installation is required.

## Java Quick Start

```java
import coakka.http.server.Responses;
import coakka.http.server.Service;
import coakka.http.server.ServiceBuilder;

Service service = new ServiceBuilder()
    .listen("127.0.0.1", 8080)
    .concurrency(2)
    .get("/hello", request -> Responses.text("Hello from CoAkka"))
    .start();

Runtime.getRuntime().addShutdownHook(new Thread(service::close));
```

Handlers use Java lambdas and public signatures contain CoAkka and JDK values.

## Kotlin Quick Start

```kotlin
import coakka.http.server.Handler
import coakka.http.server.Responses
import coakka.http.server.ServiceBuilder

val service = ServiceBuilder()
    .listen("127.0.0.1", 8080)
    .concurrency(2)
    .get("/hello", Handler { Responses.text("Hello from CoAkka") })
    .start()
```

Java and Kotlin share the same service lifecycle and capability set.

## Two Useful API Levels

| API | Best fit |
| --- | --- |
| `ServiceBuilder` and `Service` | Direct buffered JVM handlers over finite event loops, active-handler admission, and body/header/route bounds |
| `CoreConfiguration` and `HttpCore` | HTTP/1.1, HTTP/2, HTTP/3 where available, streams, trailers, SSE, WebSocket, files, outbound HTTP, TLS/mTLS, `io_uring`, inspection, handler swap, health, liveness, and monitoring |

The direct service exposes CoAkka `Request` and `Response` values rather than
network-library types. Request views are valid during the synchronous handler;
`request.bytes()` copies a body that must outlive the call, and response
factories copy caller-owned bytes. Every event and byte value returned by
`HttpCore` is copied into JVM-owned storage before the native lease is released.
One reader owns each inbound, WebSocket, outbound-terminal, or monitor-wait
lane.

## Capacity And Backpressure

The direct service owns finite event loops and bounds active handlers, request
targets, headers, request bodies, response bodies, and routes. The runtime also bounds
connections, active exchanges, chunks, sessions, files, outbound work, monitor
history, terminal observations, and shutdown waits. Pressure, limits, timeout,
cancellation, and closed state remain distinct outcomes.

Application executors, coroutine scopes, database pools, and retries also need
finite ownership. The connector cannot bound work after an application submits
it to an unrelated executor.

## Observability And Monitoring

`HttpCore` exposes `health()`, `probeLiveness()`, `monitorConfiguration()`,
`monitorSnapshot()`, generation-checked `applyMonitorPolicy()`, cursor-based
`readMonitorEvents()`, `waitForMonitor()`, and `interruptMonitorWaiter()`.

The monitor channel is bounded, reports missed history, and never retains HTTP
payloads or credentials. Read
[Observability And Monitoring](../docs/observability-and-monitoring.md) for the
data model and operator lifecycle.

## TLS, mTLS, And Live Handler Changes

`CoreConfiguration.listener(Listener(...))` accepts HTTP protocol, TLS or
mutual TLS, credential identity/generation, certificate chain, private key,
and trust roots. Supported Linux HTTP/2 and HTTP/3 configurations may select
`IoBackend.IO_URING`.

`HttpCore.rebind(...)` switches an existing route to a prepared JVM handler
binding under generation and revision checks. See
[TLS And mTLS](../docs/tls-and-mtls.md) and
[Live Handler Changes](../docs/handler-swap-and-hot-reload.md).

## Lifecycle

`Service` and `HttpCore` are `AutoCloseable`. The owner stops admission while
accepted handlers converge, closes active connections, stops its event loops,
and releases the runtime within a finite deadline. Do not block an event-loop handler
on unrelated work or call `close()` from that handler.

Use `try`/`finally` or try-with-resources around the lifecycle owner. Importing
the JAR does not install application signal policy.

## Targets And Current Gate

The private JAR contains target selections for macOS ARM64, Linux ARM64, Linux
x86-64, Windows ARM64, and Windows x86-64. Query `HttpRuntime.capabilities()`
before selecting an optional feature on the running target.

Maven publication, public sample promotion, and portable performance claims
remain closed until the corrected package and matching-host JVM evidence agree.
