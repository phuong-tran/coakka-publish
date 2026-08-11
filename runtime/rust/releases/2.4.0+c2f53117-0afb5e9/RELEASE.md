# Releasing The Rust Connector

Current source version `2.4.0` uses native generation
`2.4.0+c2f53117`. Publisher signing is absent.

## Dry-Run Gates

```sh
cargo fmt --manifest-path rust/Cargo.toml --check
cargo clippy --manifest-path rust/Cargo.toml --all-targets -- -D warnings
RUSTDOCFLAGS='-D warnings' cargo doc --manifest-path rust/Cargo.toml --no-deps
cargo test --manifest-path rust/Cargo.toml
COAKKA_FILE_LANE_RUNTIME_LIB=/absolute/path/to/runtime \
  cargo test --manifest-path rust/Cargo.toml --test file_lane -- --ignored --exact public_connector_roundtrip_crosses_native_quantum
COAKKA_STREAM_LANE_RUNTIME_LIB=/absolute/path/to/runtime \
  cargo test --manifest-path rust/Cargo.toml --test stream_lane -- --ignored --exact public_connector_stream_roundtrip
bash rust/scripts/check-package-readiness.sh
bash rust/scripts/smoke-packaged-package.sh
```

The package must contain exactly these native payloads:

- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`

Every listed payload must match the candidate ledger. The two ignored tests above are
release evidence, not optional local tests: each fails closed when its runtime
path is absent. Package presence and source build do not replace native
Linux/Windows connector and consumer execution.

`Cargo.toml` keeps `publish = false`, so this source coordinate cannot be
requested from crates.io.
