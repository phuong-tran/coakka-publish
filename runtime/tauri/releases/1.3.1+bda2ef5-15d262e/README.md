# CoAkka Runtime Tauri Intents

Tauri intent source package for the CoAkka runtime v2.

This release is built from connector source commit `15d262e` over native runtime
package generation `1.3.1+bda2ef5`.

This is not a crates.io release and not a Tauri plugin. It is a public source
package for the Rust app-host boundary:

- WebView JavaScript sends intent envelopes only
- Rust owns validation and execution
- Rust converts intent work to CoAkka runtime delivery when that is the right
  app-host path
- WebView JavaScript receives an intent result projection

## Smoke

```sh
tar -xzf coakka-runtime-tauri-intents-1.3.1-source.tar.gz
cd coakka-runtime-tauri-intents-1.3.1-source
cargo run --manifest-path examples/intent-command/Cargo.toml
```
