# Consuming `coakka-logger-node`

The Node.js logger package is a binary tarball that embeds the native logger
core. Applications normally do not need to pass a native library path.

```ts
import { CoakkaLoggerLevel, Logger } from "coakka-logger-node";

const logger = Logger.start({ minLevel: CoakkaLoggerLevel.INFO });
try {
  logger.info("orders", "accepted");
  const record = logger.awaitNext(1000);
} finally {
  logger.close();
}
```

For local development or diagnostics, an explicit native library path can be
provided:

```ts
Logger.start({}, "/abs/path/to/libcoakka_logger_core.dylib");
```

The environment override is:

```sh
COAKKA_LOGGER_LIB=/abs/path/to/libcoakka_logger_core.so
```

Current embedded native package:

- native core version: `0.1.0`
- native package version: `0.1.0+ba2a66d98eb5`
- platforms: `macos-aarch64`, `linux-aarch64`, `linux-x86_64`
