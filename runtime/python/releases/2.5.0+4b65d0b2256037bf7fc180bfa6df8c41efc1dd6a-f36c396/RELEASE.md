# Release Checklist

This checklist is for the Python connector lane only.

## Preconditions

- `coakkaCoreNativeDev/v2` builds on the current host
- the package build staging root contains the exact runtime natives
- local Python 3.11+ and `pip` are available
- the native binary contract has passed for every OS/architecture carried by
  the wheel; connector execution evidence remains platform-specific

Fetch the staged Linux natives if needed:

```sh
./python/scripts/fetch_linux_runtime_natives.sh
```

## Verify The Python Module

```sh
./python/scripts/test_package.sh
./python/scripts/smoke_packaged_wheel.sh
```

Expected result:

- the installed-package pytest suite passes, including transport ABI and
  lifecycle coverage
- packaged-wheel smoke prints `consumer_runtime_smoke ok ...`

## Build The Wheel

```sh
./python/scripts/build_wheel.sh
```

Expected wheel output:

```text
python/build/wheelhouse/
```

Expected packaged native coverage:

- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/windows-aarch64/libcoakka_runtime_v2.dll`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`

The wheel also carries versioned copies of those native libraries so the
embedded loader can prefer immutable resource names.

Use `COAKKA_V2_HOST_RUNTIME_LIB` only for a host-local dry-run. Do not infer
support from a binary being present in the archive. The distribution retains
Windows, macOS, and Linux artifacts even when a local validation run executes
only one target; release evidence and troubleshooting state the distinction.

## Verify External Consumer Shape

```sh
./python/scripts/smoke_packaged_wheel.sh
```

This step matters because it proves the installed wheel works from a clean
virtualenv instead of only from the repo source tree.

## Verify PyPI Readiness

```sh
./python/scripts/check_pypi_readiness.sh
```

This does not publish anything. It checks the package from a dependency-local
test environment, rebuilds and smokes the wheel, and verifies the wheel metadata
and embedded native platform set that a later PyPI upload would expose.

Platform publisher signing is currently absent and is recorded in package
metadata. See troubleshooting for checksum verification and platform trust
warnings.

Support: `gabrielgun1983@gmail.com` and the public issue tracker. See
[common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).
