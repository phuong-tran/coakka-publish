# Consuming CoAkka Runtime Connector For Electron

Install the published package through your normal application package feed.
Electron itself remains the application framework dependency; this package owns
the CoAkka runtime intent bridge and consumes the published Node connector as
the main-process runtime host.

Use the package only from the Electron main process and preload boundary:

- main process: start `ElectronRuntimeIntentBridge`
- preload: expose `createCoAkkaPreloadApi(ipcRenderer)`
- renderer: call `window.coakka.intent(...)`

Do not import the runtime connector directly from renderer code.
