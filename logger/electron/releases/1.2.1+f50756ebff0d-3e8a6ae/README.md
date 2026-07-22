# CoAkka Logger For Electron

This package is the Electron main-process intent bridge for CoAkka logger.

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
