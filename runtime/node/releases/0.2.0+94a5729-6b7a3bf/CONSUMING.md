# Consuming `coakka-v2-connector-node`

The Node.js runtime package is a binary tarball that embeds the native v2
runtime. Applications normally do not need to pass a native library path.

```ts
import {
  ConnectorStartSpec,
  EndpointFlag,
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

- native core version: `0.1.0`
- native package version: `0.2.0+94a5729`
- platforms: `macos-aarch64`, `linux-aarch64`, `linux-x86_64`

One Node.js process may start one active `RuntimeHost`. Close it from the app
lifecycle hook before starting another host in the same process.
