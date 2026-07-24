# CoAkka Runtime Connector For Electron

This package is the Electron main-process intent bridge for CoAkka runtime v2.

The boundary is intentionally strict:

- renderer JavaScript sends an intent through `ipcRenderer.invoke`
- preload exposes only a small `coakka.intent(...)` API
- the Electron main process owns `ElectronRuntimeIntentBridge`
- the main process converts the intent into runtime delivery and projects the
  result back to the renderer

Renderer code does not import the runtime connector, does not know runtime
envelopes, and does not own native runtime lifecycle.

## Main Process

```js
import { ipcMain } from "electron";
import {
  ElectronRuntimeIntentBridge,
  registerElectronIntentIpcHandler,
} from "coakka-v2-connector-electron";

const target = "samples.electron.intent.echo";
const bridge = ElectronRuntimeIntentBridge.start({
  systemName: "electron-app",
  nodeId: "electron-app-main",
  defaultTarget: target,
});

bridge.registerJsonIntentHandler(target, async (intent) => ({
  handledBy: "electron-main",
  echo: intent.payload,
}));

registerElectronIntentIpcHandler(ipcMain, bridge);
```

## Preload

```js
const { contextBridge, ipcRenderer } = require("electron");
const { createCoAkkaPreloadApi } = require("coakka-v2-connector-electron/preload");

contextBridge.exposeInMainWorld("coakka", createCoAkkaPreloadApi(ipcRenderer));
```

## Renderer

```js
const result = await window.coakka.intent({
  intentId: "intent-1",
  source: "electron-renderer",
  target: "samples.electron.intent.echo",
  operation: "echo",
  payload: { message: "hello-electron-runtime" },
  payloadIdentity: {
    messageType: "samples.electron.intent.echo.request.v1",
    payloadSchemaVersion: 1,
    payloadFormat: "json",
  },
  timeoutMs: 2000,
});
```

## Smoke

```sh
npm run build
bash scripts/smoke-packaged-package.sh
```

The packaged smoke installs the generated package into a disposable Electron
app, loads a hidden renderer window, sends one renderer intent through preload
and IPC, and verifies that the main process handles it through the runtime
host exposed by the published Node connector package.
