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

C#:

```csharp
using var runtime = RuntimeHost.StartLocal("csharp-practice", "hello.en");
runtime.RegisterTextHandler("hello.en", name => $"Hello {name}");

var reply = await runtime.AskTextAsync(
    source: "csharp-practice",
    target: "hello.en",
    payload: "Nam",
    payloadIdentity: PayloadIdentity.Text("hello.request.v1"),
    deliveryHint: DeliveryHint.RequireLocal);
```

Level 1 rules:

- no route snapshot in the first Kotlin local-text sample
- no endpoint host/port placeholders in first-run local samples
- no generated wire envelope in first-run samples
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

- generated wire or host-language envelope models
- typed/raw submit APIs
- delivery hints
- deadletter observation
- custom payload identity and schema versioning
- overload policy knobs

Raw APIs must be visibly raw. Typed APIs must reject missing `messageType`,
invalid schema version, or unspecified payload format.

## Language Mapping

Native C:

- Level 1 is intentionally not a C ABI goal. C callers use explicit runtime,
  frame, route, and ask-client handles.
- Public headers carry Doxygen-style ownership docs for fd ownership, buffer
  release helpers, struct-size compatibility, string lifetime, and monitor
  doorbell semantics.

C#:

- Level 1: `RuntimeHost.StartLocal`, `RegisterTextHandler`, `AskTextAsync`,
  and `PayloadIdentity.Text`.
- Level 2: `RuntimeHost.Start`, `ConnectorStartSpec`,
  `RuntimeHost.LocalRoute`, `RuntimeRouteSpec`, and `RuntimeEndpointSpec`.
- Level 3: `TransportEnvelope`, JSON helpers, explicit payload identity,
  native library resolver, and raw submit paths.
- Public C# connector APIs publish XML documentation so IntelliSense teaches
  ownership, local-route metadata, and runtime/no-discovery boundaries.

Zig:

- The current lane is source-first and intentionally narrow.
- Public `pub` structs/functions carry `///` docs for ABI lifetime,
  host-owned fds, local route metadata, and no hidden Zig event loop.
- Level 1 stays small until the lane is promoted beyond package smoke.

Mojo:

- The current lane is a low-level native smoke, not a stable application
  connector API.
- Docs say the shim owns lifecycle for package verification only until direct
  Mojo bindings are stable enough for a real Level 1 API.

Native C++ and Rust connectors should keep the same level split while using
idiomatic wrappers for each language.

## Release Rule

Each public connector release should keep its docs, samples, and package surface
aligned with these levels. Do not document a helper as released until the
published artifact actually contains it.
