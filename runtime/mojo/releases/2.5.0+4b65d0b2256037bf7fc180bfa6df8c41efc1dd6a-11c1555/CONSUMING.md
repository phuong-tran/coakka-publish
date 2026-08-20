# Consuming The Mojo Runtime Source Connector

Unpack the source distribution and run:

```sh
tar -xzf coakka-runtime-mojo-2.5.1-source.tar.gz
cd coakka-runtime-mojo-2.5.1-source
bash scripts/verify-native-payload.sh
bash scripts/smoke.sh
```

The archive contains exactly:

- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`

Override native selection with `COAKKA_RUNTIME_LIB`. The C shim owns the
runtime lifecycle for the smoke process and demonstrates structured transport
configuration without presenting the shim as a public Mojo application API.

Included native bytes and cross-compilation evidence do not imply end-to-end
Mojo execution on every target. Platform-specific loading and trust guidance
is maintained in [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).
