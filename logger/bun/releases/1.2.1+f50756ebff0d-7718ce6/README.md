# CoAkka Logger Bun

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

`logger/bun/` is the Bun host connector lane for the standalone CoAkka native
logger core.

Minimum host: Bun 1.2.0. The release gate runs both the minimum and current Bun
lines against the packaged native logger.

## New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. The logger package is the bounded native logger surface: app code emits
records, the native logger owns queueing and pressure behavior, and callers can
observe accepted, delivered, and dropped counts.

Use these public repositories to orient first:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-samples` | Runnable examples and code you can inspect first. | https://github.com/phuong-tran/coakka-samples |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |

Run the matching sample:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh logger bun basic
```

No-checkout npm smoke:
https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

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
