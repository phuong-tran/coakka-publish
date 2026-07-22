# Consuming CoAkka Tauri Intents

Unpack the source archive and use a path dependency on the intent crate:

```toml
[dependencies]
coakka-tauri-intents = { path = "/path/to/coakka-runtime-tauri-intents-1.3.1-source/coakka-tauri-intents" }
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

The package bundles native runtime libraries for:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`
