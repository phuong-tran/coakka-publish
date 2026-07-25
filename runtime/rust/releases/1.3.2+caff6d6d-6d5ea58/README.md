# CoAkka Rust Runtime Connector

This is the Rust connector package for the CoAkka runtime v2.

macOS, Linux, and Windows are packaged runtime targets for this connector lane.
Linux remains the default deployment path.

The first slice proves the Rust connector can:

- load the native CoAkka runtime library on the current packaged macOS, Linux,
  and Windows lanes
- start one local `RuntimeHost`
- apply one route snapshot
- register a local handler
- send one request/reply call
- surface a route-miss deadletter
- enable the delivered-request lane by default for request/reply hosts

The current package keeps the API synchronous and local-first. Tokio
integration, crates.io metadata, generated wire codecs, and broader package
surface work remain follow-on packaging work.

`ConnectorStartSpec::new(...)` defaults `separate_delivered_request_lane` to
`true`. Most request/reply services should leave it alone so inbound handler
work stays separate from reply/deadletter matching. Set it to `false` only for
advanced, measured, mostly one-way hosts.

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

The current package stages the same native runtime generation as the other
runtime connector lanes.
