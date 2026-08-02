# Consuming CoAkka Runtime Rust Connector

Source version `1.4.0` uses exact native generation
`1.4.0+2cee86bf`. It is a local source coordinate, not a crates.io release.

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
coakka-runtime-rs = { path = "/path/to/coakka-runtime-rs-1.4.0" }
```

Run the packaged smoke:

```sh
cargo run --manifest-path /path/to/coakka-runtime-rs-1.4.0/Cargo.toml --bin coakka-rust-smoke
```

The crate resolves native libraries from:

1. explicit `runtime_lib_path`
2. `COAKKA_RUNTIME_LIB`
3. packaged `native/<platform>/`
4. repo-local fallback paths used during local development

Supported packaged platforms for this connector:

- `macos-aarch64`
- `linux-aarch64`
- `windows-x86_64`

All three payloads must match the candidate ledger. Current execution evidence
covers macOS ARM64; Linux ARM64 and Windows x86-64 remain native
connector/consumer execution gates. The Windows DLL stays in the distribution.
Use [Transport Configuration](TRANSPORT_CONFIGURATION.md) and [common troubleshooting](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/troubleshooting.md)
for loader, OS/CPU, dependency, certificate, Gatekeeper, Authenticode, digest,
and currently absent publisher-signing failures.

## Boundary Shape

Keep HTTP at real application edges. Internal Rust work should be a runtime
target when the goal is request/reply, route ownership, deadletter behavior,
and polyglot participation without inventing another backend HTTP service.
