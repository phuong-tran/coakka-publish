# Consuming CoAkka Runtime Native v2 C ABI

Unpack the archive into your dependency directory:

```sh
tar -xzf coakka-runtime-native-v2-<version>.tar.gz
```

## C ABI

```cmake
set(CMAKE_PREFIX_PATH "/path/to/coakka-runtime-native-v2-<version>")
find_package(CoAkkaRuntimeNativeV2 CONFIG REQUIRED)

add_executable(runtime_c main.c)
target_link_libraries(runtime_c PRIVATE CoAkkaRuntimeNativeV2::runtime_v2)
```

The C ABI is declared by:

- `include/coakka/v2/runtime.h`
- `include/coakka/v2/control.h`
- `include/coakka/v2/client.h`
- `include/coakka/v2/transport.h`
- `include/coakka/v2/utils.h`

## Platform Selection

The CMake config selects one of these platform directories from the host build:

- `native/linux-aarch64`
- `native/linux-x86_64`
- `native/macos-aarch64`

Each platform directory contains exactly one loadable runtime shared library:
`libcoakka_runtime_v2` with the host suffix for that OS.

If your deployment target differs, build the native runtime for that target and
add a matching platform directory before packaging your app.

## Runtime Loader

The executable must be able to find `libcoakka_runtime_v2` at runtime. During
development, use `DYLD_LIBRARY_PATH` on macOS or `LD_LIBRARY_PATH` on Linux:

```sh
DYLD_LIBRARY_PATH=/path/to/coakka-runtime-native-v2-<version>/native/macos-aarch64 ./runtime_c
LD_LIBRARY_PATH=/path/to/coakka-runtime-native-v2-<version>/native/linux-x86_64 ./runtime_c
```

Normal public packages are self-contained. Consumers should not need to install
ambient `protobuf`, `absl`, `CAF`, `libuv`, or any other third-party native
runtime dependency for this archive.
