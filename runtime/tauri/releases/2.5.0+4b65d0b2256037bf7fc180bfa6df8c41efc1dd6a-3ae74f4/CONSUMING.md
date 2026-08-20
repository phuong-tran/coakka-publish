# Consuming CoAkka Tauri Intents

This lane is a source package, not a crates.io release and not a Tauri plugin.

Unpack the archive and use a path dependency on the intent crate:

```toml
[dependencies]
coakka-tauri-intents = { path = "/path/to/coakka-runtime-tauri-intents-2.5.2-source/coakka-tauri-intents" }
```

A Tauri command should keep the WebView boundary narrow:

```rust
#[tauri::command]
fn coakka_ask_intent(
    intent: coakka_tauri_intents::IntentEnvelope,
    state: tauri::State<'_, coakka_tauri_intents::RuntimeIntentDispatcher>,
) -> Result<coakka_tauri_intents::IntentResult, coakka_tauri_intents::IntentError> {
    coakka_tauri_intents::coakka_ask_intent_command(&state, intent)
}
```

Frontend code sends only intent envelopes through Tauri `invoke`. It should not
construct runtime transport envelopes, own native runtime handles, or call FFI.

The package bundles scanner-clean native runtime libraries for this release:

- `macos-aarch64`
- `linux-aarch64`
- `windows-x86_64`

The bundled `examples/intent-command` crate proves the command-level path
without launching a WebView:

```sh
cargo run --manifest-path examples/intent-command/Cargo.toml
```

For the cross-language model, connection strategies, TLS/mTLS, and runtime
troubleshooting, use the
[CoAkka documentation hub](https://github.com/phuong-tran/coakka-samples/tree/main/docs).
