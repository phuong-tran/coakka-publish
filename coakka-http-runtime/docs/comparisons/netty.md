# CoAkka HTTP Runtime And Netty

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and the Netty 4.x
documentation current on the comparison date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Different Boundaries](#different-boundaries)
- [Pressure And Monitoring](#pressure-and-monitoring)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

Netty applications assemble event loops, a server bootstrap, channels, and a
pipeline of handlers:

```java
ServerBootstrap bootstrap = new ServerBootstrap()
    .group(bossGroup, workerGroup)
    .channel(NioServerSocketChannel.class)
    .childHandler(new ChannelInitializer<SocketChannel>() {
        protected void initChannel(SocketChannel channel) {
            channel.pipeline().addLast(httpCodec, applicationHandler);
        }
    });

bootstrap.bind(3000).sync();
```

This is powerful because the application can assemble protocol and channel
behavior at a low level.

## The CoAkka Shape

```java
Service service = new ServiceBuilder()
    .listen("127.0.0.1", 3000)
    .get("/api/hello", request -> Responses.text("Hello from CoAkka"))
    .start();
```

CoAkka starts from an application-level HTTP service rather than asking code to
assemble a channel pipeline. Routes and handlers remain JVM code; the
connector owns service bounds, health, monitoring state, and lifecycle.

## Different Boundaries

| Netty | CoAkka HTTP Runtime |
| --- | --- |
| General asynchronous network application framework | Operational HTTP runtime |
| Channels, pipelines, handlers, futures | Routes, response values, typed runtime events, and monitor snapshots |
| Application assembles codecs and pipeline | Connector supplies the HTTP service contract |
| JVM-focused | Eight public language surfaces |
| Suitable for custom protocols | Focused on the published HTTP contract |

CoAkka is not a replacement for Netty when the application needs to design a
custom protocol or control a channel pipeline. It is more direct when the goal
is a shared HTTP service contract rather than a new network stack.

## Pressure And Monitoring

Netty exposes channel writability and configurable write-buffer watermarks;
event-loop and application task admission remain part of the system design.

CoAkka defines finite connection, handler, body, stream, session, and
diagnostic capacities, explicit pressure outcomes, health, and bounded service
snapshots. The packaged `HttpRuntime` API also exposes a bounded monitor event
channel with cursor and missed-history accounting.

## Choose By Responsibility

Choose Netty for deep JVM networking control, custom protocols, or systems
already built around channels and pipelines.

Choose CoAkka for HTTP applications that value a consistent bounded lifecycle
across several languages and prefer route/service APIs over protocol assembly.

## Official References

- [Netty project](https://netty.io/)
- [Netty 4.x user guide](https://netty.io/wiki/user-guide-for-4.x.html)
- [Netty channel configuration](https://netty.io/4.2/api/io/netty/channel/ChannelConfig.html)
- [CoAkka How It Works](../how-it-works.md)
