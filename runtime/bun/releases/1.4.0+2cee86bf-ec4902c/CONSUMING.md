# Consuming The Bun Connector

This is the Bun connector for the polyglot, multi-language, multi-platform
CoAkka Runtime ecosystem.

Current shape:

- package name: `coakka-v2-connector-bun`
- public runtime owner alias: `BunRuntimeHost`
- client alias: `BunRuntimeClient`
- native runtime ABI: shared `libcoakka_runtime_v2`
- implementation base: the shared JS runtime connector core compiled at package
  build time

The package reuses the shared JavaScript connector core while keeping Bun host
ergonomics and package validation separate. Runtime semantics remain owned by
the native core and public C ABI.

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
