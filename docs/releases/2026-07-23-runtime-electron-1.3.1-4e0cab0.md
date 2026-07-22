# 2026-07-23 Runtime Electron 1.3.1 Refresh 4e0cab0

This release adds the Electron runtime connector lane over native runtime
package `1.3.1+bda2ef5`.

Published artifact:

- Electron package: `runtime/electron/releases/1.3.1+bda2ef5-4e0cab0/coakka-v2-connector-electron-1.3.1.tgz`

The package is an Electron main-process bridge. Renderer JavaScript sends an
intent through preload and IPC; the Electron main process owns runtime
lifecycle, handles the intent through CoAkka runtime delivery, and returns a
projected result to the renderer.

Verification before publish:

```sh
bash scripts/verify_bun_tauri_release_train.sh

python3 scripts/verify-runtime-intake-artifact.py --lane electron \
  --artifact runtime/electron/releases/1.3.1+bda2ef5-4e0cab0/coakka-v2-connector-electron-1.3.1.tgz \
  --expected-native-version 1.3.1+bda2ef5

bash scripts/verify-public-surface.sh
```

The connector release train includes:

- connector matrix
- Bun packaged smoke
- Electron packaged smoke
- Bun sample
- Tauri command sample
- Tauri desktop sample
- Electron desktop intent sample
- sample artifact guard tests
