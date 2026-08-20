# CoAkka Tauri Intents

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

This crate is the Rust core for the Tauri intent source-package lane.

Minimum Rust version: 1.77.2. The intent crate targets the Tauri 2 application
shape but deliberately has no direct Tauri dependency, so it does not force a
particular `cargo-tauri` CLI build onto consumers. A Tauri app may require a
higher Rust toolchain through its own dependency graph.

Tauri is one integration lane in the wider multi-language, multi-platform
[CoAkka ecosystem](https://github.com/phuong-tran/coakka-samples/tree/main/docs).

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
`coakka-publish/runtime/tauri/releases/2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a-0afb5e9/`.

## File Lane Boundary

The WebView intent API intentionally does not expose file paths, transfer
tokens, or native file-lane methods. Trusted Rust host state may use the Rust
connector directly under the shared [file-lane contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-file-transfer.md).

## Stream Lane Boundary

Do not infer that the Tauri WebView intent package exposes Stream Lane. The
`2.5.2-source` aggregate package gives the trusted Rust host the owner-aware
Rust File and Stream Lane APIs, including `open_owned`, `prepare_*_grant`, and
`from_control_plane`. WebView JavaScript still receives no path, callback, or
bearer grant. Follow the [owner-grant contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-lane-owner-grants.md).

## AI-Assisted Integration

Before generating Tauri integration code, read the public
[AI-assisted integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ai-assisted-integration.md)
and the runnable Tauri sample. WebView JavaScript remains intent-only; the Rust
app host owns any runtime, File Lane, or source-candidate Stream Lane work.

## License

**Free for application use, including commercial and production use.**

The Tauri intent adapter source uses the
[Apache License, Version 2.0](https://github.com/phuong-tran/coakka-samples/blob/main/LICENSE).
The aggregate source archive also contains the Rust runtime connector and
bundled Native Core files. Those native files use the
[CoAkka Native Artifact License 1.2](https://github.com/phuong-tran/coakka-samples/blob/main/NATIVE-LICENSE.md).
Those native terms permit ordinary application and SaaS use but require a
separate agreement to sell or offer CoAkka itself as managed runtime or
infrastructure. See
[CoAkka Package Licensing](https://github.com/phuong-tran/coakka-samples/blob/main/docs/package-licensing.md)
for the file-scope map. The aggregate archive carries offline `LICENSE`,
`NATIVE-LICENSE.md`, `PACKAGE-LICENSE.md`, and `NOTICE` copies, and the nested
source adapter carries its Apache-2.0 `LICENSE` copy.
