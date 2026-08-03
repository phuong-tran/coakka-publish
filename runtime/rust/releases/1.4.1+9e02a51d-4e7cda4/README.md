# CoAkka Rust Runtime Connector

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

This is the Rust connector in the polyglot, multi-language, multi-platform
CoAkka runtime ecosystem. CoAkka is not Rust-only: Rust services can share the
same runtime semantics with JVM, Node/Bun, Python, Go, C#, Swift, Zig, Mojo, and
native C/C++ applications.

Source version `1.4.1` uses native generation
`1.4.1+9e02a51d`.
macOS, Linux, and Windows remain packaged runtime targets. Linux is the primary
deployment path, but narrower local evidence never removes another OS payload.

Common guidance:

- [CoAkka documentation and samples](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
- [Connection strategies](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/connection-strategies.md)
- [TLS and mTLS](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/troubleshooting.md)
- [Contact and support](https://github.com/phuong-tran/coakkaCoreNativeDev/blob/main/v2/docs/common/contact-and-support.md): `gabrielgun1983@gmail.com`
- [Rust transport API](TRANSPORT_CONFIGURATION.md)

The connector proves it can:

- resolve and compile the OS-specific native loader path for current packaged
  macOS, Linux, and Windows lanes; native execution evidence is stated below
- start one local `RuntimeHost`
- apply one route snapshot
- register a local handler
- send one request/reply call
- surface a route-miss deadletter
- enable the delivered-request lane by default for request/reply hosts
- discover effective capabilities before optional-mode selection
- apply connection/security policy before start and return copied structured
  results
- reload a newer TLS/mTLS credential generation atomically while preserving
  active state after rejection
- retain one native module identity for process lifetime and close every
  host-owned exported fd exactly once

The current package keeps the API synchronous and local-first.

`ConnectorStartSpec::new(...)` defaults `separate_delivered_request_lane` to
`true`. Most request/reply services should leave it alone so inbound handler
work stays separate from reply/deadletter matching. Set it to `false` only for
advanced, measured, mostly one-way hosts.

## Run

```sh
cargo run --manifest-path rust/Cargo.toml --bin coakka-rust-smoke
cargo run --manifest-path rust/Cargo.toml --bin coakka-rust-transport-smoke
```

The resolver checks:

1. explicit `runtime_lib_path`
2. `COAKKA_RUNTIME_LIB`
3. `rust/native/<platform>/`
4. `go/native/<platform>/`
5. repo-local `lib/`

The staged tarball contains exact digest-verified native payloads for macOS
ARM64, Linux ARM64, and Windows x86-64. Current native Rust execution evidence
is macOS ARM64. Cross-platform source compilation and payload presence do not
replace Linux/Windows connector and consumer execution, which is not yet
recorded.
