# Consuming The Zig Runtime Source Connector

Unpack the release-shaped source tarball and run:

```sh
tar -xzf coakka-runtime-zig-2.4.0-source.tar.gz
cd coakka-runtime-zig-2.4.0-source
bash scripts/verify-native-payload.sh
bash scripts/smoke.sh
```

The archive contains exactly:

- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`

Override native selection with `COAKKA_RUNTIME_LIB`. The first exact path
loaded remains resident for process lifetime; a different later path is
rejected. This prevents unload/reload from re-registering native static state.

This remains a source package, not a stable package-manager artifact. Included
bytes and cross-compilation do not prove native execution on another target.
