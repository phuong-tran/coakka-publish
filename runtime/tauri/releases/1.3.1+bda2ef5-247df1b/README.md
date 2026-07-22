# CoAkka Tauri Intents

This crate is the Rust core for the Tauri intent source-package lane.

The boundary is intentionally strict:

- WebView JavaScript sends intent envelopes.
- Rust validates and executes intents.
- Rust converts intents into CoAkka runtime envelopes when runtime delivery is
  the right execution path.
- JavaScript receives a projected result and renders it.

This crate does not depend on Tauri yet. It exposes a Tauri-shaped bridge
function that a real `#[tauri::command]` wrapper can call while keeping the
core crate testable without a Tauri app scaffold.

```rust
#[tauri::command]
fn coakka_ask_intent(
    intent: coakka_tauri_intents::IntentEnvelope,
    state: tauri::State<'_, coakka_tauri_intents::RuntimeIntentDispatcher>,
) -> Result<coakka_tauri_intents::IntentResult, coakka_tauri_intents::IntentError> {
    coakka_tauri_intents::coakka_ask_intent_command(&state, intent)
}
```

Rust app code can register JSON handlers through `RuntimeIntentDispatcher`
without exposing runtime transport envelopes to WebView JavaScript.
The dispatcher is the Rust-side state owner; when the last dispatcher clone is
dropped, the underlying runtime host is dropped and stops the native runtime.

`IntentEnvelope.headers` is reserved app-host metadata in this first slice. It
is validated before the runtime boundary, but it is not forwarded into the
runtime request envelope until the Rust connector opens a header-aware ask
surface. JavaScript code should treat headers as intent metadata, not transport
metadata.

## Smoke

```sh
cargo test --manifest-path tauri-intents/Cargo.toml
```

The integration test starts the native runtime through the Rust connector,
registers one local intent handler, submits one intent through the command
bridge function, and verifies the projected result.

## Package

```sh
bash tauri-intents/scripts/package-release.sh
```

The package is a source archive with bundled native runtime libraries and a
command-level smoke under `examples/intent-command`.

The public package is published under
`coakka-publish/runtime/tauri/releases/1.3.1+bda2ef5-247df1b/`.
