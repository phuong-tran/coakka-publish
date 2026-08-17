# Consuming `coakka-logger-bun`

The Bun logger package embeds the native logger core. Applications normally do
not need to pass a native library path or install third-party native runtime
dependencies.

```ts
import { BunLogger, CoakkaLoggerLevel } from "coakka-logger-bun";

const logger = BunLogger.start({ minLevel: CoakkaLoggerLevel.INFO });
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
BunLogger.start({}, "/abs/path/to/libcoakka_logger_core.dylib");
```

The environment override is:

```sh
COAKKA_LOGGER_LIB=/abs/path/to/libcoakka_logger_core.so
```

Current embedded native package:

- native core version: `1.2.1`
- native package version: `1.2.1+f50756ebff0d`
- platforms: `macos-aarch64`, `linux-aarch64`, `linux-x86_64`, `windows-aarch64`, `windows-x86_64`
