# CoAkka Runtime Connector For Bun

This is the first Bun lane for the CoAkka runtime v2 connector.

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

The lane is deliberately thin for the first slice:

- it packages a Bun-named surface: `coakka-v2-connector-bun`
- it exports `BunRuntimeHost`, `BunConnectorOrchestrator`, and
  `BunRuntimeClient` aliases
- it compiles the shared JS runtime connector core during package build
- it ships the same staged native runtime files inside the Bun package

Runtime semantics still belong to the native runtime and its public C ABI. Bun
owns host ergonomics, package smoke, and compatibility evidence only.

## Local Commands

```sh
export BUN_INSTALL="$HOME/.bun"
export PATH="$BUN_INSTALL/bin:$PATH"

bun run build
bun run verify
bash scripts/smoke-packaged-package.sh
```

The package smoke builds a tarball, installs it into a temporary Bun consumer,
starts one embedded runtime, registers a local JSON handler, and proves
request/reply through the native runtime.

## Boundary

This lane currently reuses the shared JS runtime connector core because Bun can
run the embedded native binding path. If that boundary stops holding, the
replacement should be a narrow Bun binding shim, not a fork of runtime routing,
request/reply, deadletter, or lifecycle semantics.
