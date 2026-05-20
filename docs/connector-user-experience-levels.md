# Connector User Experience Levels

This document is the public connector onboarding contract for current and
upcoming CoAkka runtime connector releases.

Most application users should learn connector APIs first, not the native runtime
core. The runtime still owns route application, routing, deadletters, lifecycle,
and diagnostics. Connectors own the host-language experience above that ABI.

## Level 1: Local Handler And Ask

First-run samples should start here.

Kotlin:

```kotlin
val runtime = CoAkka.local("kotlin-practice")
runtime.handler("hello.en") { name -> "Hello $name" }
val reply = runtime.ask("hello.en", "Nam")
```

Java:

```java
runtime.registerTextHandler("hello.en", name -> "Hello " + name);
String reply = runtime.getJava().askTextBlocking("java-client", "hello.en", "Nam");
```

Node.js:

```ts
const requestIdentity = PayloadIdentity.text("demo.echo.request.v1");
const routes = [localRoute("svc.echo")];
```

Python:

```python
request_identity = PayloadIdentity.text("demo.echo.request.v1")
routes = [local_route("svc.echo")]
```

Go:

```go
identity := connector.NewTextPayloadIdentity("demo.echo.request.v1")
routes := []connector.RouteSpec{connector.LocalRouteDefault("svc.echo")}
```

Level 1 rules:

- no route snapshot in the first Kotlin local-text sample
- no endpoint host/port placeholders in first-run local samples
- no generated protobuf envelope in first-run samples
- no explicit payload identity for plain-text local Kotlin/Java helpers
- no runtime-owned discovery, watch loop, or hidden application runner

## Level 2: Explicit Route Snapshot

Control-plane and framework adapters use this layer.

Examples:

- `ConnectorOrchestrator`
- `RuntimeStartSpec`
- `RuntimeRouteSpec`
- typed endpoint flags and route flags
- snapshot generation and stale-generation rejection

The connector or app host owns service inventory and generation bumps. Runtime
does not watch Kubernetes, service discovery, or config stores by itself.

## Level 3: Custom Envelope And Transport Policy

Connector authors and advanced integrations use this layer.

Examples:

- generated protobuf or host-language envelope models
- typed/raw submit APIs
- delivery hints
- deadletter observation
- custom payload identity and schema versioning
- overload policy knobs

Raw APIs must be visibly raw. Typed APIs must reject missing `messageType`,
invalid schema version, or unspecified payload format.

## Release Rule

Each public connector release should keep its docs, samples, and package surface
aligned with these levels. Do not document a helper as released until the
published artifact actually contains it.
