# CoAkka HTTP Runtime And Eclipse Jetty

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and Eclipse Jetty
`12.1.x`, the current documentation line on the comparison date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Mental Model](#mental-model)
- [Threading And Pressure](#threading-and-pressure)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

Jetty applications can compose a `Server`, connectors, and handlers directly:

```java
Server server = new Server(3000);
server.setHandler(new Handler.Abstract.NonBlocking() {
    @Override
    public boolean handle(Request request, Response response, Callback callback) {
        response.setStatus(200);
        Content.Sink.write(response, true, "Hello from Jetty", callback);
        return true;
    }
});
server.start();
```

Jetty also supports Jakarta EE applications and a broad collection of server
components.

## The CoAkka Shape

```java
Service service = new ServiceBuilder()
    .listen("127.0.0.1", 3000)
    .get("/api/hello", request -> Responses.text("Hello from CoAkka"))
    .start();
```

## Mental Model

| Jetty | CoAkka HTTP Runtime |
| --- | --- |
| Server, connectors, handler tree | language connector and route handlers |
| Jetty handler or Jakarta EE APIs | CoAkka builder API |
| JVM server/client libraries | CoAkka contract projected across eight language surfaces |
| Component lifecycle, dumps, JMX and metrics integrations | Health, liveness, bounded monitor snapshots/events, and application exporters |
| HTTP protocol options | Host-specific CoAkka package capability |

Jetty offers a larger JVM protocol and container surface today. CoAkka's
distinguishing boundary is not "more Jetty"; it is one operational HTTP runtime
projected into several host languages.

## Threading And Pressure

Jetty documents why its thread-pool queue may need to run protocol-critical
tasks and why simply making that queue small can stop server progress. Jetty
therefore treats thread execution and HTTP concurrency as different concerns.

CoAkka uses a different application boundary: connections, active handlers,
bodies, streams, sessions, and diagnostics have explicit capacities and
refusal or terminal outcomes. This is not evidence that one model is
universally faster.

## Choose By Responsibility

Choose Jetty for a composable JVM server/client library, Jakarta EE support, or
active HTTP/2 and HTTP/3 requirements.

Choose CoAkka for a shared cross-language HTTP contract, explicit pressure,
health, and bounded monitoring state. Use an addon when the JVM application
wants higher-level framework conventions.

## Official References

- [Jetty HTTP server libraries](https://jetty.org/docs/jetty/12.1/programming-guide/server/http.html)
- [Jetty threading architecture](https://jetty.org/docs/jetty/12/programming-guide/arch/threads.html)
- [Jetty 12.1 programming guide](https://jetty.org/docs/jetty/12.1/programming-guide/index.html)
- [CoAkka Operations](../operations.md)
