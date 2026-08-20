# CoAkka Runtime Connector For Bun

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

**This is the Bun connector in the polyglot, multi-language, multi-platform
CoAkka Runtime ecosystem.** CoAkka is not a Bun-only runtime: this package
adapts Bun applications to the same native core, public C ABI, target,
request/reply, bounded-admission, and deadletter contract used by the JVM,
Node.js, Python, Go, C#, Rust, Swift, and other connector lanes.

Kubernetes is supported but not required. Use the public
[Ecosystem Overview](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ecosystem-overview.md)
and [Compatibility Matrix](https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md)
to select the exact package, OS, CPU, and release-channel tuple.
Start with the [CoAkka Documentation](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
for concepts, integration paths, operations, and runnable samples.

Minimum host: Bun 1.2.0. Both the floor and current Bun are exercised by the
release gate; Node.js is used only by package build tooling and is not a Bun
consumer runtime requirement.

## New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. It helps an app route work by target name, handle request/reply,
deadletters, bounded queues, diagnostics, and native-backed logging without
turning every internal boundary into another hand-written HTTP endpoint.

Use these public repositories to orient first:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-samples` | Runnable examples and code you can inspect first. | https://github.com/phuong-tran/coakka-samples |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |

Run the matching sample:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh runtime bun basic
```

No-checkout npm smoke:
https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

That smoke starts from the same question most new users have: if the app
currently creates fake backend HTTP only to call work owned by the same app or
team, the CoAkka after shape is a runtime target such as
`samples.customer.store.create`, not another private URL.

The Bun adapter remains deliberately thin:

- it packages a Bun-named surface: `coakka-v2-connector-bun`
- it exports `BunRuntimeHost`, `BunConnectorOrchestrator`, and
  `BunRuntimeClient` aliases
- it compiles the shared JS runtime connector core during package build
- it ships the exact native runtime payloads recorded by package metadata

Runtime semantics still belong to the native runtime and its public C ABI. Bun
owns host ergonomics, package smoke, and compatibility evidence only.

The package now exposes the same capability, startup connection/TLS, structured
apply, and atomic credential reload API as Node. See
[Connection Strategies](https://github.com/phuong-tran/coakka-samples/blob/main/docs/connection-strategies.md),
[TLS and mTLS](https://github.com/phuong-tran/coakka-samples/blob/main/docs/tls-and-mtls.md),
and [Troubleshooting](https://github.com/phuong-tran/coakka-samples/blob/main/docs/troubleshooting.md).

## Local Commands

```sh
export BUN_INSTALL="$HOME/.bun"
export PATH="$BUN_INSTALL/bin:$PATH"

bun run build
bun run verify
bash scripts/smoke-packaged-package.sh
```

Host-library consumer smoke:

```sh
COAKKA_V2_HOST_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.dylib \
  bun run smoke:packaged
```

The package smoke builds a tarball, installs it into a temporary Bun consumer,
starts one embedded runtime, registers a local JSON handler, and proves
request/reply through the native runtime.

## Boundary

This lane currently reuses the shared JS runtime connector core because Bun can
run the embedded native binding path. If that boundary stops holding, the
replacement should be a narrow Bun binding shim, not a fork of runtime routing,
request/reply, deadletter, or lifecycle semantics.

Contact: `gabrielgun1983@gmail.com`.

## File Lane

`FileLane.open(...)` is available through a Bun-native FFI adapter. Its native
wait blocks the calling event loop, so run waits in a Worker or dedicated
process. See the shared [file-lane contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-file-transfer.md).

## Stream Lane

Version `2.5.0` contains the Stream Lane connector surface over exact native
generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. Keep that pairing intact and follow the public
[streaming contract](https://github.com/phuong-tran/coakka-samples/blob/main/docs/runtime-streaming.md).

## AI-Assisted Integration

Before generating application code, use the selected connector README together
with the public [AI-assisted integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/ai-assisted-integration.md).
It requires an exact package coordinate, platform evidence, the runnable
language sample, and the feature-specific lifecycle contract. Do not translate
API identifiers from another language by analogy.

## License

Connector material is Apache-2.0 under [LICENSE](LICENSE). Bundled native
material uses the terms in [NATIVE-LICENSE.md](NATIVE-LICENSE.md). See
[PACKAGE-LICENSE.md](PACKAGE-LICENSE.md) for the file-scope map and
[NOTICE](NOTICE) for the package notice.
