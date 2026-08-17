# Consuming CoAkka Tauri Logger Intents

This lane is a source package, not a crates.io release and not a Tauri plugin.

Unpack the archive and use a path dependency on the intent crate:

```toml
[dependencies]
coakka-tauri-logger-intents = { path = "/path/to/coakka-logger-tauri-intents-1.2.2-source/coakka-tauri-logger-intents" }
```

A Tauri command should keep the WebView boundary narrow:

```rust
#[tauri::command]
fn coakka_log_intent(
    intent: coakka_tauri_logger_intents::LoggerIntent,
    state: tauri::State<'_, coakka_tauri_logger_intents::TauriLoggerIntentBridge>,
) -> Result<
    coakka_tauri_logger_intents::LoggerIntentResult,
    coakka_tauri_logger_intents::LoggerIntentError,
> {
    coakka_tauri_logger_intents::coakka_log_intent_command(&state, intent)
}
```

Frontend code sends only log intents through Tauri `invoke`. It should not own
native logger handles or call native loading APIs.

The package bundles logger natives for:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`
