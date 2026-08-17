# CoAkka Logger Node

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

`logger/node/` is the Node.js host connector lane for the standalone CoAkka
native logger core.

Minimum host: Node.js 22. Release gates also cover the current Node.js LTS
line without raising the declared floor.

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
bash run.sh logger node basic
```

No-checkout npm smoke:
https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

Try the npm package without cloning any CoAkka repo:

```sh
mkdir coakka-logger-first-run
cd coakka-logger-first-run
npm init -y
npm install coakka-logger-node@1.2.7
```

```js
import { CoakkaLoggerLevel, Logger } from "coakka-logger-node";

const logger = Logger.start({
  systemName: "first-user-logger",
  minLevel: CoakkaLoggerLevel.INFO,
});

try {
  const sequence = logger.info("first.user", JSON.stringify({ hello: "logger" }));
  const record = logger.awaitNext(1000);
  if (sequence == null || record == null) {
    throw new Error("expected one accepted and drained log record");
  }
  console.log({ sequence: record.sequence, category: record.category });
} finally {
  logger.close();
}
```

It mirrors the Python logger lane:

- load `libcoakka_logger_core` through `koffi`
- keep Node-side formatting above the native core
- let the native logger own queueing, pressure policy, sinks, and lifecycle
- package staged native libraries into the npm tarball for macOS aarch64,
  Linux aarch64, and Linux x86_64

## Build

```sh
npm --prefix logger/node run build
```

The script stages native libraries from:

```text
logger/staging/native/1.2.1+f50756ebff0d/
```

## Smoke

```sh
npm --prefix logger/node test
npm --prefix logger/node run smoke:packaged
```

## Example

```ts
import { Logger } from "coakka-logger-node";

const logger = Logger.start();
try {
  logger.info("app", "started");
  const record = logger.awaitNext(1000);
} finally {
  logger.close();
}
```
