# Consuming CoAkka Runtime Rust Connector

Source version `2.1.0` uses exact native generation
`2.1.0+60ddf70d`. It is a local source coordinate, not a crates.io release.

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
coakka-runtime-rs = { path = "/path/to/coakka-runtime-rs-2.1.0" }
```

Run the packaged smoke:

```sh
cargo run --manifest-path /path/to/coakka-runtime-rs-2.1.0/Cargo.toml --bin coakka-rust-smoke
```

The crate resolves native libraries from:

1. explicit `runtime_lib_path`
2. `COAKKA_RUNTIME_LIB`
3. packaged `native/<platform>/`
4. repo-local fallback paths used during local development

Supported packaged platforms for this connector:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

All five payloads must match the release ledger. Version 2.1.0 passes native
request/reply and file-lane execution on Linux ARM64/x86-64 and uses the
target-correct Linux `RTLD_LOCAL` value. Both Windows DLLs stay in the
distribution with verification evidence but no matching Rust-on-Windows
execution record.
Use [Transport Configuration](TRANSPORT_CONFIGURATION.md) and [common troubleshooting](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/troubleshooting.md)
for loader, OS/CPU, dependency, certificate, Gatekeeper, Authenticode, digest,
and currently absent publisher-signing failures.

## Boundary Shape

Keep HTTP at real application edges. Internal Rust work should be a runtime
target when the goal is request/reply, route ownership, deadletter behavior,
and polyglot participation without inventing another backend HTTP service.
