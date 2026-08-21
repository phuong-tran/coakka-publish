# CoAkka Runtime Connector For Electron

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

**This is the Electron connector in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem.** CoAkka is not an Electron-only
runtime: this package keeps native runtime ownership in the main process while
preserving the same target, request/reply, bounded-admission, and deadletter
contract used by Node.js, the JVM, Python, Go, C#, Rust, Swift, and other
connector lanes.

Kubernetes is supported but not required. Use the public
[Ecosystem Overview](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ecosystem-overview.md)
and [Compatibility Matrix](https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md)
to select an exact package and native platform tuple.
Start with the [CoAkka Documentation](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
for concepts, integration paths, operations, and runnable samples.

Minimum hosts: Electron 42 and Node.js 22 in the Electron main process. The
native runtime remains owned by main-process code; renderer and preload code
do not load it directly.

## New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. It helps an app route work by target name, handle request/reply,
deadletters, bounded queues, diagnostics, and native-backed logging without
turning every internal boundary into another hand-written HTTP endpoint.

Use these public repositories to orient first:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-samples` | Runnable examples and code you can inspect first. | https://github.com/phuong-tran/coakka-samples |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |

Run the matching sample:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh runtime electron basic
```

No-checkout npm smoke:
https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

That smoke explains the runtime mental model first: keep real HTTP or IPC at
the app edge, but replace fake backend HTTP used only for work owned by the
same app or team with a CoAkka target and explicit reply/deadletter behavior.

The boundary is intentionally strict:

- renderer JavaScript sends an intent through `ipcRenderer.invoke`
- preload exposes only a small `coakka.intent(...)` API
- the Electron main process owns `ElectronRuntimeIntentBridge`
- the main process converts the intent into runtime delivery and projects the
  result back to the renderer

Renderer code does not import the runtime connector, does not know runtime
envelopes, and does not own native runtime lifecycle.

The main-process options can forward the shared `connectionStrategy` and
`security` startup specs. Capability/config/security getters and atomic apply
results are exposed on `ElectronRuntimeIntentBridge`; renderer/preload code
still never receives credential material. See
[Connection Strategies](https://github.com/phuong-tran/coakka-samples/blob/main/docs/connection-strategies.md),
[TLS and mTLS](https://github.com/phuong-tran/coakka-samples/blob/main/docs/tls-and-mtls.md),
and [Troubleshooting](https://github.com/phuong-tran/coakka-samples/blob/main/docs/troubleshooting.md).

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
and IPC, and verifies that the main process handles it through the runtime host
exposed by its packaged Node connector dependency.

Package metadata records the exact native generation, release channel, and
supported platform tuple. Contact: `gabrielgun1983@gmail.com`.

## File Lane

The Node dependency exposes `FileLane` to trusted Electron main-process code.
Do not forward raw file paths, authorization tokens, or lane methods through
preload/renderer IPC. See the shared [file-lane contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-file-transfer.md).

## Stream Lane

Version `2.5.3` contains owner-aware File and Stream Lane connector surfaces for Electron main
over exact native generation `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`. Keep runtime ownership in the
main process and follow the public
[streaming contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-streaming.md).
Electron re-exports Node's `FileReceiveGrant`, `StreamPublishGrant`, and
`openOwned`/`prepare*Grant` methods only to main-process code. Follow the
[owner-grant ONE/ALL sample](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-lane-owner-grants.md)
and keep serialized grants out of preload and renderer IPC.

## AI-Assisted Integration

Before generating Electron integration code, read the public
[AI-assisted integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ai-assisted-integration.md)
and the runnable Electron sample. Connection policy and native lane ownership
belong to Electron main; renderer code sends validated intent only.


## License

**Free for application use, including commercial and production use.**

Connector source, generated bindings, type declarations, examples, and package
documentation use the [Apache License, Version 2.0](https://github.com/phuong-tran/coakka-samples/blob/main/LICENSE).
Bundled Native Core files use the [CoAkka Native Artifact License 1.2](https://github.com/phuong-tran/coakka-samples/blob/main/NATIVE-LICENSE.md).
Those native terms permit ordinary application and SaaS use but require a
separate agreement to sell or offer CoAkka itself as managed runtime or
infrastructure.

See [CoAkka Package Licensing](https://github.com/phuong-tran/coakka-samples/blob/main/docs/package-licensing.md)
for the file-scope map. The package also carries offline `LICENSE`,
`NATIVE-LICENSE.md`, `PACKAGE-LICENSE.md`, and `NOTICE` copies.
