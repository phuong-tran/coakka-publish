# CoAkka Runtime Connector For Bun

This is the first Bun lane for the CoAkka runtime v2 connector.

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
