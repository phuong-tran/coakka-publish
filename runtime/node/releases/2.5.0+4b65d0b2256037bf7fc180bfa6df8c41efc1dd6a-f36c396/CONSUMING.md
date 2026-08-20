# Consuming `coakka-v2-connector-node`

This is the Node.js connector for the polyglot, multi-language, multi-platform
CoAkka Runtime ecosystem. The package embeds the native v2 runtime, so
applications normally do not need to pass a native library path.

Import the connector through the package root. Transport framing modules are
internal and are not supported deep-import entrypoints.

```ts
import {
  ConnectorStartSpec,
  EndpointFlag,
  localRoute,
  PayloadFormat,
  PayloadIdentity,
  RuntimeHost,
} from "coakka-v2-connector-node";
```

For local development or diagnostics, an explicit native library path can be
provided:

```ts
RuntimeHost.start(startSpec, "/abs/path/to/libcoakka_runtime_v2.so");
```

The environment override is:

```sh
COAKKA_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.so
```

Current embedded native package:

- native core version: `2.5.0`
- bundled native generation:
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`
- exact npm `2.5.0` request/reply executes on macOS ARM64 and Linux
  ARM64/x86-64
- Windows ARM64/x86-64 payloads are verified package contents; matching
  Node.js-on-Windows execution is not recorded for this release

One Node.js process may start one active `RuntimeHost`. Close it from the app
lifecycle hook before starting another host in the same process.

First-run helpers:

```ts
const requestIdentity = PayloadIdentity.text("demo.echo.request.v1");
const routes = [localRoute("svc.echo")];
```

Use `PayloadFormat.TEXT` for plain text in user-facing code. `PLAIN_TEXT`
remains as a compatibility alias for the legacy wire-format name.

Transport lifecycle, ownership, blocking, atomicity, capability, and error
contracts are documented in the public
[connection strategy](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md)
and [TLS/mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
guides.
