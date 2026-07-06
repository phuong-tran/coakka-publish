# Consuming The Mojo Logger Source Connector

Unpack the archive and run the smoke with Mojo installed:

```sh
tar -xzf coakka-logger-mojo-1.2.1-source.tar.gz
cd coakka-logger-mojo-1.2.1-source
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

This source connector currently proves the logger ABI entrypoint only:
`coakka_logger_core_get_abi_version`. It is a source package for evaluation and
sample execution, not a stable package-manager artifact.
