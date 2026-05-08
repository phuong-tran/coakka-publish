# Consuming CoAkka Runtime Rust Spike

This Rust connector package is an early package artifact. It is not a crates.io
release and should not be presented as stable API yet.

The package proves the local-first runtime path:

- `RuntimeHost::start(...)`
- route snapshot at startup
- local handler registration
- JSON request/reply with typed payload identity
- matched route-miss deadletter
- runtime info/config and client counters

## Local Tarball

Unpack the release tarball and use a path dependency:

```toml
[dependencies]
coakka-runtime-rs = { path = "/path/to/coakka-runtime-rs-0.1.0-spike" }
```

Run the packaged smoke:

```sh
cargo run --manifest-path /path/to/coakka-runtime-rs-0.1.0-spike/Cargo.toml --bin coakka-rust-smoke
```

The crate resolves native libraries from:

1. explicit `runtime_lib_path`
2. `COAKKA_RUNTIME_LIB`
3. packaged `native/<platform>/`
4. repo-local fallback paths used during local development

Supported packaged platforms for this spike:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`

Windows support is not claimed.

## Boundary Shape

Keep HTTP at real application edges. Internal Rust work should be a runtime
target when the goal is request/reply, route ownership, deadletter behavior,
and polyglot participation without inventing another internal REST service.
