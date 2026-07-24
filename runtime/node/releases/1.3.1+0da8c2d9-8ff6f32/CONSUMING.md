# Consuming `coakka-v2-connector-node`

The Node.js runtime package is a binary tarball that embeds the native v2
runtime. Applications normally do not need to pass a native library path.

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

- native core version: `1.3.6`
- native package version: `1.3.6+0da8c2d9`
- platforms: `macos-aarch64`, `linux-aarch64`, `linux-x86_64`,
  `windows-aarch64`, `windows-x86_64`

One Node.js process may start one active `RuntimeHost`. Close it from the app
lifecycle hook before starting another host in the same process.

First-run helpers:

```ts
const requestIdentity = PayloadIdentity.text("demo.echo.request.v1");
const routes = [localRoute("svc.echo")];
```

Use `PayloadFormat.TEXT` for plain text in user-facing code. `PLAIN_TEXT`
remains as a compatibility alias for the wire name.
