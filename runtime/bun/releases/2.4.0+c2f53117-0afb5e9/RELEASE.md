# Release Checklist

This checklist is for the `coakka-v2-connector-bun` lane only.

## Preconditions

- the package build staging root contains the exact native inputs
- Bun is available, either on `PATH` or under `~/.bun/bin`
- Node.js is available for the package build script

## Verify The Bun Package

```sh
bun --version
bun --revision
bun --cwd bun run verify
bun --cwd bun run smoke:packaged
```

Expected result:

- shared JS runtime core compiles into `bun/dist/core`
- Bun test passes
- packaged-tarball smoke prints `consumer_runtime_smoke ok ...`

## Build The Tarball

```sh
bun --cwd bun run pack:release
```

Expected package output:

```text
bun/coakka-v2-connector-bun-2.4.0.tgz
```

Expected packaged native coverage:

- `dist/native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `dist/native/linux-aarch64/libcoakka_runtime_v2.so`
- `dist/native/linux-x86_64/libcoakka_runtime_v2.so`
- `dist/native/windows-aarch64/libcoakka_runtime_v2.dll`
- `dist/native/windows-x86_64/libcoakka_runtime_v2.dll`

The tarball also carries versioned copies of those native libraries so the
embedded loader can prefer immutable resource names.

## Host Override

A host-only dry-run may use `COAKKA_V2_HOST_RUNTIME_LIB=/abs/path`. It is
evidence for that host only and does not substitute for connector execution on
another platform.
