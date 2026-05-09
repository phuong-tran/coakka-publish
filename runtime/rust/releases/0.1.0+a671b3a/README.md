# CoAkka Rust Runtime Spike

This is an early connector package for the CoAkka runtime v2.

The first slice proves the Rust connector can:

- load the native CoAkka runtime library on macOS/Linux
- start one local `RuntimeHost`
- apply one route snapshot
- register a local handler
- send one request/reply call
- surface a route-miss deadletter

The spike keeps the API synchronous and local-first. Tokio integration,
packaging, crates.io metadata, generated protobufs, and public docs are later
work.

## Run

```sh
cargo run --manifest-path rust/Cargo.toml --bin coakka-rust-smoke
```

The resolver checks:

1. explicit `runtime_lib_path`
2. `COAKKA_RUNTIME_LIB`
3. `rust/native/<platform>/`
4. `go/native/<platform>/`
5. repo-local `lib/`

The current spike reuses already staged native libraries from the Go connector
lane instead of copying a second native bundle.
