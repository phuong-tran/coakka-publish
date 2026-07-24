# Consuming The Zig Runtime Source Connector

Unpack the archive and run the smoke with Zig installed:

```sh
tar -xzf coakka-runtime-zig-1.3.6-source.tar.gz
cd coakka-runtime-zig-1.3.6-source
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
delivered-request lane, and route-miss deadletter through the ask-client path.
It is a source package, not a stable package-manager artifact.
