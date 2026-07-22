# CoAkka Tauri Logger Intents

This crate is the Rust core for the Tauri logger intent source-package lane.

The boundary is intentionally strict:

- WebView JavaScript sends log intents.
- Rust validates and executes log intents.
- Rust owns the logger lifecycle and queue pressure behavior.
- JavaScript receives a projected result with accepted/dropped counters.

This crate does not depend on Tauri yet. It exposes a Tauri-shaped bridge
function that a real `#[tauri::command]` wrapper can call while keeping the
core crate testable without a Tauri app scaffold.

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

Rust app code owns `TauriLoggerIntentBridge`. WebView JavaScript should not
construct native logger handles or call native loading APIs.

## Smoke

```sh
cargo test --manifest-path logger/tauri-intents/Cargo.toml
```

## Package

```sh
bash logger/tauri-intents/scripts/package-release.sh
```
