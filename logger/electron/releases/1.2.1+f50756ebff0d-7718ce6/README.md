# CoAkka Logger For Electron

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

This package is the Electron main-process intent bridge for CoAkka logger.

Minimum hosts: Electron 42 and Node.js 22 in the Electron main process.

## New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. The Electron logger package keeps renderer JavaScript on an intent API
while the Electron main process owns native logger lifecycle, bounded queueing,
drain, and counters.

Use these public repositories to orient first:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-samples` | Runnable examples and code you can inspect first. | https://github.com/phuong-tran/coakka-samples |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |

Run the matching sample:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh logger electron basic
```

No-checkout npm smoke:
https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

The boundary is intentionally strict:

- renderer JavaScript sends a log intent through `ipcRenderer.invoke`
- preload exposes only a small `coakkaLogger.log(...)` API
- the Electron main process owns `ElectronLoggerIntentBridge`
- the main process submits the log record and projects counters back to the
  renderer

Renderer code does not import the logger connector, does not own native logger
lifecycle, and does not need a native library path in the normal path.

## Main Process

```js
import { ipcMain } from "electron";
import {
  ElectronLoggerIntentBridge,
  registerElectronLoggerIpcHandler,
} from "coakka-logger-electron";

const bridge = ElectronLoggerIntentBridge.start({
  systemName: "electron-app-logger",
});

registerElectronLoggerIpcHandler(ipcMain, bridge);
```

## Preload

```js
const { contextBridge, ipcRenderer } = require("electron");
const { createCoAkkaLoggerPreloadApi } = require("coakka-logger-electron/preload");

contextBridge.exposeInMainWorld("coakkaLogger", createCoAkkaLoggerPreloadApi(ipcRenderer));
```

## Renderer

```js
const result = await window.coakkaLogger.log({
  intentId: "log-1",
  source: "electron-renderer",
  level: "info",
  category: "app.orders",
  message: '{"event":"accepted"}',
  awaitDrainMs: 1000,
});
```

## Smoke

```sh
npm run build
bash scripts/smoke-packaged-package.sh
```
