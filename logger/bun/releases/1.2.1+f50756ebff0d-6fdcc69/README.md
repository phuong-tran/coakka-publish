# CoAkka Logger Bun

`logger/bun/` is the Bun host connector lane for the standalone CoAkka native
logger core.

It mirrors the Node.js logger lane while using Bun's native FFI surface instead
of a Node native dependency:

- load `libcoakka_logger_core` through `bun:ffi`
- keep Bun-side formatting above the native core
- let the native logger own queueing, pressure policy, sinks, and lifecycle
- package staged native libraries into the tarball for macOS, Linux, and
  Windows aarch64/x86_64

## Build

```sh
bun --cwd logger/bun run build
```

The script stages native libraries from:

```text
logger/staging/native/1.2.1+f50756ebff0d/
```

## Smoke

```sh
bun --cwd logger/bun test
bun --cwd logger/bun run smoke:packaged
```

## Example

```ts
import { BunLogger } from "coakka-logger-bun";

const logger = BunLogger.start();
try {
  logger.info("app", "started");
  const record = logger.awaitNext(1000);
} finally {
  logger.close();
}
```
