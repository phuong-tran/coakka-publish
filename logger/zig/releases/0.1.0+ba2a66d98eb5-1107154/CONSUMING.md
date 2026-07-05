# Consuming The Zig Logger Source Connector

Unpack the archive and run the smoke with Zig installed:

```sh
tar -xzf coakka-logger-zig-0.1.0-source.tar.gz
cd coakka-logger-zig-0.1.0-source
bash scripts/smoke.sh
```

The package bundles scanner-clean native logger libraries for:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`

`scripts/smoke.sh` first prefers packaged native libraries under
`native/<platform>/`, then falls back to repo-local staged natives during
source checkout work. Override native loading explicitly with:

```sh
COAKKA_LOGGER_LIB=/path/to/libcoakka_logger_core bash scripts/smoke.sh
```

This source connector proves create/start/emit/read/stop over the public logger
C ABI. It is a source package for evaluation and sample execution, not a stable
package-manager artifact.
