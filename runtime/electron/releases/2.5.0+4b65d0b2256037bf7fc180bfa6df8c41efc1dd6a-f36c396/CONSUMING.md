# Consuming CoAkka Runtime Connector For Electron

This is the Electron connector for the polyglot, multi-language, multi-platform
CoAkka Runtime ecosystem. Install it through your normal application package
feed. Electron remains the application framework dependency; this package owns
the CoAkka runtime intent bridge and consumes the Node connector as the
main-process runtime host.

Use the package only from the Electron main process and preload boundary:

- main process: start `ElectronRuntimeIntentBridge`
- preload: expose `createCoAkkaPreloadApi(ipcRenderer)`
- renderer: call `window.coakka.intent(...)`

Do not import the runtime connector directly from renderer code.
