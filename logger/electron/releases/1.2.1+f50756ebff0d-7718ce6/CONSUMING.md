# Consuming CoAkka Logger For Electron

Install the published package through your normal application package feed.
Electron itself remains the application framework dependency; this package owns
the CoAkka logger intent bridge and consumes the published Node logger package
from the Electron main process.

Use the package only from the Electron main process and preload boundary:

- main process: start `ElectronLoggerIntentBridge`
- preload: expose `createCoAkkaLoggerPreloadApi(ipcRenderer)`
- renderer: call `window.coakkaLogger.log(...)`

Do not import the logger connector directly from renderer code.
