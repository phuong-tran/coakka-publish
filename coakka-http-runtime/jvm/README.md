# CoAkka HTTP Runtime for Java and Kotlin

![CoAkka](https://raw.githubusercontent.com/phuong-tran/coakka-samples/a539dfcd7dc8d93c5b1c61b1324899af13d47ca6/docs/common/assets/brand/coakka-logo.png)

CoAkka HTTP Runtime is an embeddable HTTP server and client for Java and
Kotlin. Serve HTTP APIs, stream responses, handle WebSockets and call
upstream services with JVM handlers. Both languages use the same managed
JAR and the native HTTP core for the JVM's platform.

The CoAkka ecosystem provides polyglot tools for distributed applications,
HTTP services and logging. CoAkka HTTP Runtime is the HTTP product and can
be adopted independently.

[CoAkka ecosystem](https://github.com/phuong-tran/coakka-publish#choose-your-product) · [Raspberry Pi 5 benchmarks](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/benchmark/README.md)

## Requirements

Use JDK 17 for the examples below. Install the package with Maven or Gradle. The connector JAR targets Java 8
bytecode. Pick the native ZIP for the operating system and architecture of
the JVM that will run your app. Check the architecture with
`java -XshowSettings:properties -version` and look for `os.arch`.

| Platform | JVM architecture | Native ZIP classifier |
| --- | --- | --- |
| macOS 11 or newer | ARM64 | `macos-aarch64` |
| Linux, glibc 2.28 or newer | ARM64 | `linux-aarch64` |
| Linux, glibc 2.28 or newer | x86-64 | `linux-x86_64` |
| Windows | ARM64 | `windows-aarch64` |
| Windows | x64 | `windows-x86_64` |

On WSL2, use the Linux classifier for the JVM running inside WSL2.

## Install 2.2.1

For a new Maven app, save this as `pom.xml`. In an existing Maven app, add
the dependency to your `dependencies` block.

```xml
<project xmlns="http://maven.apache.org/POM/4.0.0">
    <modelVersion>4.0.0</modelVersion>
    <groupId>example</groupId>
    <artifactId>coakka-echo</artifactId>
    <version>1.0.0</version>
    <dependencies>
        <dependency>
            <groupId>io.github.phuong-tran.coakka</groupId>
            <artifactId>coakka-http-runtime-jvm</artifactId>
            <version>2.2.1</version>
        </dependency>
    </dependencies>
</project>
```

For an existing Gradle app, use Maven Central and add the same dependency
in `build.gradle.kts`. Maven and Gradle resolve the Kotlin standard library
as a transitive dependency, including for Java applications.

```kotlin
repositories {
    mavenCentral()
}

dependencies {
    implementation("io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.2.1")
}
```

**Maven:** Download and unpack the matching native ZIP. Run the following as a single
command from the directory containing your Maven project. Replace
`macos-aarch64` with the classifier for your JVM.

```sh
mvn org.apache.maven.plugins:maven-dependency-plugin:3.8.1:unpack "-Dartifact=io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.2.1:zip:macos-aarch64" "-DoutputDirectory=vendor/coakka-http"
```

The ZIP creates `vendor/coakka-http/native`. Pass that directory with
`-Djava.library.path` at JVM startup. In Windows PowerShell, add it to the
process `PATH` before starting the application.

```powershell
$env:PATH = "$(Resolve-Path vendor/coakka-http/native);$env:PATH"
```

Use the same version for the JAR and the native ZIP. If the JVM reports
`UnsatisfiedLinkError`, check the classifier, the extracted `native` directory,
and the value of `java.library.path` in the application process.

**Gradle:** Resolve the ZIP from Maven Central as a separate dependency.
Replace `macos-aarch64` with the classifier for your JVM:

```kotlin
val coakkaNative by configurations.creating
val coakkaNativeRoot = layout.buildDirectory.dir("coakka-http")

dependencies {
    add(coakkaNative.name, "io.github.phuong-tran.coakka:coakka-http-runtime-jvm:2.2.1:macos-aarch64@zip")
}

val unpackCoakkaNative by tasks.registering(Sync::class) {
    from(coakkaNative.elements.map { files -> zipTree(files.single().asFile) })
    into(coakkaNativeRoot)
}

tasks.named<JavaExec>("run") {
    dependsOn(unpackCoakkaNative)
    standardInput = System.`in`
    val nativeDir = coakkaNativeRoot.get().dir("native").asFile.absolutePath
    jvmArgs("-Djava.library.path=$nativeDir")
    if (System.getProperty("os.name").startsWith("Windows")) {
        environment("PATH", "$nativeDir;${System.getenv("PATH").orEmpty()}")
    }
}
```

The `run` task requires Gradle's `application` plugin. Run
`./gradlew run` for that app, or `./gradlew unpackCoakkaNative` when
launching from an IDE; point the IDE's JVM at `build/coakka-http/native`
and add it to the process PATH on Windows. In PowerShell, use
`.\gradlew.bat`. The standalone commands below use the Maven example's
directory layout.

## First request in Java

From the example Maven project, copy the runtime JARs into `vendor/lib`.

```sh
mvn org.apache.maven.plugins:maven-dependency-plugin:3.8.1:copy-dependencies "-DincludeScope=runtime" "-DoutputDirectory=vendor/lib"
```

Save this as `EchoServer.java` in the project directory.

```java
import coakka.http.Responses;
import coakka.http.Service;
import coakka.http.ServiceBuilder;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.TimeoutException;

public final class EchoServer {
    private EchoServer() {}

    public static void main(String[] args)
            throws IOException, InterruptedException, TimeoutException {
        try (Service service = new ServiceBuilder()
                .listen("127.0.0.1", 0)
                .post("/echo", request -> Responses.bytes(request.bytes(), 201))
                .start()) {
            System.out.printf("Listening on http://127.0.0.1:%d%n", service.getPort());
            System.out.println("Press Enter to stop.");
            BufferedReader input = new BufferedReader(
                    new InputStreamReader(System.in, StandardCharsets.UTF_8));
            input.readLine();
        }
    }
}
```

Compile and run on macOS or Linux.

```sh
javac -cp "vendor/lib/*" EchoServer.java
java "-Djava.library.path=vendor/coakka-http/native" -cp ".:vendor/lib/*" EchoServer
```

In Windows PowerShell, use `".;vendor/lib/*"` as the classpath in the `java`
command. Send a request from another terminal using the port printed by the
server. For example, if it prints port 49321, use the following command.

```sh
curl -i --data "hello" http://127.0.0.1:49321/echo
```

Use `curl.exe` in Windows PowerShell. Expect HTTP 201 with the body `hello`.
Press Enter in the server terminal to close the service.

## First request in Kotlin

Use the same Maven project, downloaded JARs, and extracted ZIP. Put the
[Kotlin 2.3.10 command-line compiler](https://github.com/JetBrains/kotlin/releases/tag/v2.3.10)
on your `PATH`. Save this as `EchoServer.kt` in the project directory.

```kotlin
import coakka.http.Responses
import coakka.http.ServiceBuilder

fun main() {
    ServiceBuilder()
        .listen(host = "127.0.0.1", port = 0)
        .post(pattern = "/echo") { request ->
            Responses.bytes(request.bytes(), status = 201)
        }
        .start()
        .use { service ->
            println("Listening on http://127.0.0.1:${service.port}")
            println("Press Enter to stop.")
            readlnOrNull()
        }
}
```

Compile with `kotlinc` and run on macOS or Linux.

```sh
kotlinc -jvm-target 17 EchoServer.kt -classpath vendor/lib/coakka-http-runtime-jvm-2.2.1.jar -d echo-kotlin.jar
java "-Djava.library.path=vendor/coakka-http/native" -cp "echo-kotlin.jar:vendor/lib/*" EchoServerKt
```

In Windows PowerShell, use `"echo-kotlin.jar;vendor/lib/*"` as the classpath
in the `java` command. Send the same `/echo` request using the port printed
by the Kotlin server. It returns HTTP 201 with the body `hello`. Press Enter
to close the service.

## Choose an HTTP version

`listen(host, port)` in the Java and Kotlin examples starts plain HTTP/1.1.
For HTTP/2 or HTTP/3, generate the
[local TLS files](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/README.md#prepare-local-tls-files) and call
`listener(...)` with a `Listener`. Kotlin example:

```kotlin
import coakka.http.Listener
import coakka.http.ListenerProtocol
import coakka.http.ServiceBuilder
import coakka.http.TransportSecurity

val builder = ServiceBuilder().listener(
    Listener(
        bindAddress = "127.0.0.1",
        port = 8443,
        protocol = ListenerProtocol.HTTP_2,
        security = TransportSecurity.TLS,
        credentialGeneration = 1,
        credentialId = "local-example",
        certificateChainFile = "http-test-tls/server.pem",
        privateKeyFile = "http-test-tls/server.key",
    )
)
```

Register routes on `builder`, then start the service. Use
`ListenerProtocol.HTTP_3` to serve HTTP/3 over UDP. The complete
[Java](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/Server.java) and [Kotlin](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/protocol-listeners/Server.kt) examples show
both choices. The JVM 2.2.1 connector selects standalone listeners;
it does not expose the same-port V1 profile.

[HTTP/1.1](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http1.md) · [HTTP/2](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http2.md) · [HTTP/3](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/http3.md) · [All 3 on one port](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/guides/http-versions/multi.md)

## API reference

- [HTTP Runtime documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime)
- [Feature recipes and coverage](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/reference/FEATURES.md)
- [URL grammar and glossary](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/reference/glossary.md)
- [Archived 1.0.0 feature recipes](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/kotlin/integration.md#4-try-the-http-features)

`ServiceBuilder` creates one service. Use Java's try-with-resources or
Kotlin's `use` to close it. `close()` stops admission and waits for in-flight
work within the configured deadline. If an admitted handler has yet to stop
and cleanup reports retained ownership, allow that handler to finish and
retry close.

## I/O backend

The runtime uses the default backend for your platform. On Linux you can
request io_uring explicitly.

```kotlin
ServiceBuilder()
    .ioBackend(IoBackend.IO_URING)
    .get("/health", Handler { Responses.empty() })
    .start()
    .use { service ->
        val info = service.runtimeInfo()
        println("requested=${info.ioUringRequested}")
        println("effective=${info.ioUringEffective}")
        println("fallback=${info.fallbackReason}")
    }
```

The runtime checks whether it can apply the request and uses the platform
default otherwise. Read `ioUringEffective` and `fallbackReason` to see the
backend in use and the outcome of your request.

## Supported service features

The service builder accepts nullable `Compression` intent for eligible buffered
responses; `Limits.transportTimeouts` groups header/body/keep-alive/protocol-idle
deadlines. The runtime resolves omitted/zero values and validates bounds. Streaming
remains unbuffered even when buffered GZIP is enabled. Read `effectiveLimits()`
for accepted deadlines, not the input values.

`service.routes` (Java: `service.getRoutes()`) returns a coherent immutable runtime
`RouteSnapshot`, independently of monitoring, not local registration declarations.
Outbound terminals provide named reason/phase/retry/certainty enums with raw values
preserved separately. Unknown values are `UNRECOGNIZED`; an HTTP error response
is still `OutboundReason.RESPONSE`, not a transport failure.

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
