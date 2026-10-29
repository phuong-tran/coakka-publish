# Logger Tauri And Electron 1.2.1 Refresh

This publishes the Tauri and Electron logger connector lanes over native
logger package generation `1.2.1+f50756ebff0d` from connector source commit
`3e8a6ae`.

Published artifacts:

| Artifact | Version | SHA256 |
| --- | --- | --- |
| `logger/electron/releases/1.2.1+f50756ebff0d-3e8a6ae/coakka-logger-electron-1.2.1.tgz` | `1.2.1` | `167d92be0649fcd9600669d4e8af2e79b24a39344a18d39261d132a76dbc0417` |
| `logger/tauri/releases/1.2.1+f50756ebff0d-3e8a6ae/coakka-logger-tauri-intents-1.2.1-source.tar.gz` | `1.2.1-source` | `013a18bb1c9c7aeb598ecc1783e630d03ea9e6b25b52ed3c3eac774dde1a93f4` |

The Electron package is a main-process intent bridge over the published Node.js
logger package. Renderer code sends log intents through preload/IPC and does
not own logger lifecycle.

The Tauri package is a Rust source package. WebView JavaScript sends log
intents through a Tauri command, while Rust owns validation, queue pressure,
logger lifecycle, and result projection.

Validation:

- Electron package build
- Electron packaged consumer smoke
- Tauri Rust tests
- Tauri source-package command smoke
- public surface scan

GitHub Release upload target:
`coakka-public-artifacts-v1.3.1`.
