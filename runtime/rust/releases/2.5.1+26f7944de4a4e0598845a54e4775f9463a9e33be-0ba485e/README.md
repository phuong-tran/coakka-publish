# CoAkka Rust Runtime Connector

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

This is the Rust connector in the polyglot, multi-language, multi-platform
CoAkka runtime ecosystem. CoAkka is not Rust-only: Rust services can share the
same runtime semantics with JVM, Node/Bun, Python, Go, C#, Swift, Zig, Mojo, and
native C/C++ applications.

Source version `2.5.3` uses native generation
`2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`.
macOS, Linux, and Windows remain packaged runtime targets. Linux is the primary
deployment path, but narrower local evidence never removes another OS payload.

Common guidance:

- [CoAkka documentation and samples](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
- [Connection strategies](https://github.com/phuong-tran/coakka-samples/blob/main/docs/connection-strategies.md)
- [TLS and mTLS](https://github.com/phuong-tran/coakka-samples/blob/main/docs/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakka-samples/blob/main/docs/troubleshooting.md)
- [Contact and support](https://github.com/phuong-tran/coakka-samples/blob/main/docs/contact-and-support.md): `gabrielgun1983@gmail.com`
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

The package contains exact digest-verified native payloads for macOS ARM64,
Linux ARM64/x86-64, and Windows ARM64/x86-64. Version 2.5.3 passes native
request/reply and file-lane execution on both Linux architectures and uses
target-specific loader constants. Windows payloads remain verified without a
matching Rust-on-Windows execution claim.

## File Lane

`FileLane::open(...)` exposes a `Send + Sync` independent native bulk-transfer
lane with checked ABI snapshot projection and draining `Drop`. See the shared
[file-lane contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-file-transfer.md).

## Stream Lane

Version `2.5.3` contains the Stream Lane connector surface over exact native
generation `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`. Keep that pairing intact and follow the public
[streaming contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-streaming.md).
Replica owners use `FileLane::open_owned`/`StreamLane::open_owned` and
`prepare_receive_grant`/`prepare_publish_grant`. Use the explicit
`from_control_plane` constructors after deserialization; `Debug` keeps tokens
redacted. Derive jobs with `to_send_spec`/`to_subscribe_spec` and follow the
[owner-grant ONE/ALL sample](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-lane-owner-grants.md).

## AI-Assisted Integration

Before generating application code, use the selected connector README together
with the public [AI-assisted integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ai-assisted-integration.md).
It requires an exact package coordinate, platform evidence, the runnable
language sample, and the feature-specific lifecycle contract. Do not translate
API identifiers from another language by analogy.


## License

**Free for application use, including commercial and production use.**

Connector source, generated bindings, type declarations, examples, and package
documentation use the [Apache License, Version 2.0](https://github.com/phuong-tran/coakka-samples/blob/main/LICENSE).
Bundled Native Core files use the [CoAkka Native Artifact License 1.2](https://github.com/phuong-tran/coakka-samples/blob/main/NATIVE-LICENSE.md).
Those native terms permit ordinary application and SaaS use but require a
separate agreement to sell or offer CoAkka itself as managed runtime or
infrastructure.

See [CoAkka Package Licensing](https://github.com/phuong-tran/coakka-samples/blob/main/docs/package-licensing.md)
for the file-scope map. The package also carries offline `LICENSE`,
`NATIVE-LICENSE.md`, `PACKAGE-LICENSE.md`, and `NOTICE` copies.
