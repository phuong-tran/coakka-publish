# Consuming The Bun Connector

This lane is the Bun-facing package for the CoAkka runtime v2 connector.

Current shape:

- package name: `coakka-v2-connector-bun`
- public runtime owner alias: `BunRuntimeHost`
- client alias: `BunRuntimeClient`
- native runtime ABI: shared `libcoakka_runtime_v2`
- implementation base: the shared JS runtime connector core compiled at package
  build time

The first cut intentionally reuses the shared JS runtime connector core because
Bun 1.3.14 can run the packaged native runtime path. If Bun-specific
event-loop or package-manager behavior later needs a different binding, this
lane can replace only the native binding layer without changing runtime core
semantics.

## Minimal Smoke

```ts
import {
  BunRuntimeHost,
  BunRuntimeClient,
  DeliveryHint,
  EndpointFlag,
  PayloadFormat,
  PayloadIdentity,
} from "coakka-v2-connector-bun";
```

Use `BunRuntimeHost.start(...)` the same way Node samples use
`RuntimeHost.start(...)` or `ConnectorOrchestrator.start(...)`.
