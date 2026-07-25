# Consuming The Mojo Runtime Source Connector

Unpack the archive and run the smoke with Mojo and a C compiler installed:

```sh
tar -xzf coakka-runtime-mojo-1.3.2-source.tar.gz
cd coakka-runtime-mojo-1.3.2-source
bash scripts/smoke.sh
```

The package bundles scanner-clean native runtime libraries for:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`

`scripts/smoke.sh` first prefers packaged native libraries under
`native/<platform>/`, then falls back to repo-local staged natives during
source checkout work. Override native loading explicitly with:

```sh
COAKKA_RUNTIME_LIB=/path/to/libcoakka_runtime_v2 bash scripts/smoke.sh
```

This source connector proves lifecycle/control, raw request/reply through the
delivered-request lane, and route-miss deadletter through a sample-local C
shim. It is a source package, not a stable package-manager artifact.
