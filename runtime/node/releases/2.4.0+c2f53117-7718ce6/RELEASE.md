# Release Checklist

This checklist is for the `coakka-v2-connector-node` lane only.

## Preconditions

- the package build staging root contains the exact native inputs
- local Node.js and npm are available

## Verify The Node Module

```sh
npm --prefix node run verify
npm --prefix node run smoke:packaged
```

Expected result:

- TypeScript build passes
- tests pass
- packaged-tarball smoke prints `consumer_runtime_smoke ok ...`

## Build The Tarball

```sh
npm --prefix node run pack:release
```

Expected package output:

```text
node/coakka-v2-connector-node-2.4.1.tgz
```

Expected packaged native coverage:

- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `native/windows-aarch64/libcoakka_runtime_v2.dll`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`

The tarball also carries versioned copies of those native libraries so the
embedded loader can prefer immutable resource names.

Use `COAKKA_V2_HOST_RUNTIME_LIB=/abs/path` only for a host-only package dry-run.
It is evidence for that host only and does not substitute for connector
execution on another platform.
