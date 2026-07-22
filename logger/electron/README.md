# Logger Electron Artifacts

The current Electron logger package is published under:

- `logger/electron/releases/1.2.1+f50756ebff0d-3e8a6ae/coakka-logger-electron-1.2.1.tgz`

It is a main-process intent bridge over the published Node.js logger package.
Renderer JavaScript sends log intents through preload/IPC; the Electron main
process owns logger lifecycle and pressure counters.
