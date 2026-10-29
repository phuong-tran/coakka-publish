# CoAkka HTTP Runtime And Apache Tomcat

**Compared: 2026-09-13.** Scope: CoAkka HTTP Runtime `1.0.0` and Apache Tomcat
`11.0.25`, whose official connector reference was current on the comparison
date.

## Contents

- [The Familiar Shape](#the-familiar-shape)
- [The CoAkka Shape](#the-coakka-shape)
- [Mental Model](#mental-model)
- [Capacity And Operations](#capacity-and-operations)
- [Choose By Responsibility](#choose-by-responsibility)
- [Official References](#official-references)

## The Familiar Shape

Tomcat commonly hosts Jakarta Servlet applications:

```java
@WebServlet("/api/hello")
public final class HelloServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws IOException {
        response.setContentType("text/plain");
        response.getWriter().write("Hello from Tomcat");
    }
}
```

The application may be deployed to a Tomcat installation or run through a
framework that embeds Tomcat.

## The CoAkka Shape

```java
Service service = new ServiceBuilder()
    .listen("127.0.0.1", 3000)
    .get("/api/hello", request -> Responses.text("Hello from CoAkka"))
    .start();
```

There is no Servlet API in the CoAkka programming model. The JVM connector
invokes ordinary handlers and owns the CoAkka service lifecycle
inside the App Host.

## Mental Model

| Tomcat | CoAkka HTTP Runtime |
| --- | --- |
| Web server and Jakarta Servlet container | CoAkka HTTP service and language connectors |
| Servlet, Filter, Listener, deployment model | Builder handlers and optional addons |
| Connector plus Catalina container | CoAkka JVM service inside the App Host |
| Java/JVM applications | Eight public language surfaces |
| JMX and surrounding integrations | Health, liveness, bounded monitor snapshots/events, and application exporters |

## Capacity And Operations

Tomcat documents connector controls such as processing threads, maximum
connections, and the operating-system accept queue. When processing capacity is
busy, work may wait at different connection and thread boundaries.

CoAkka exposes finite connection, active-handler, retained-byte, stream,
session, diagnostic, and shutdown capacities as one product contract. The models are
different; a single "bounded or unbounded" label is not a fair comparison.

Tomcat `11` also supports protocol and Servlet capabilities beyond CoAkka's
builder model. Exact protocol support is recorded per final application package.

## Choose By Responsibility

Choose Tomcat for Jakarta Servlet applications, traditional deployments, or
frameworks already integrated with the Servlet container model.

Choose CoAkka when the same HTTP, pressure, monitoring, and shutdown semantics
must extend beyond the JVM or when a small route-and-handler service is enough.

## Official References

- [Tomcat HTTP Connector](https://tomcat.apache.org/tomcat-11.0-doc/config/http.html)
- [Tomcat connectors guide](https://tomcat.apache.org/tomcat-11.0-doc/connectors.html)
- [CoAkka Capabilities](../capabilities.md)
- [CoAkka Operations](../operations.md)
