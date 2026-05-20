# Consuming The Zig Runtime Source Connector

Unpack the archive and run the smoke with Zig installed:

```sh
tar -xzf coakka-runtime-zig-0.2.0-source.tar.gz
cd coakka-runtime-zig-0.2.0-source
bash scripts/smoke.sh
```

The package bundles scanner-clean native runtime libraries for Linux `aarch64`,
Linux `x86_64`, and macOS `aarch64`. You can override native loading with
`COAKKA_RUNTIME_LIB=/path/to/libcoakka_runtime_v2`.

This source connector proves lifecycle/control, raw request/reply through the
delivered-request lane, and route-miss deadletter through the ask-client path.
It is a source package, not a stable package-manager artifact.
