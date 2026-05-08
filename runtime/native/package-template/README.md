# CoAkka Runtime Native v2 C ABI

This package contains the public runtime v2 C ABI headers and prebuilt native
runtime libraries for:

- `linux-aarch64`
- `linux-x86_64`
- `macos-aarch64`

The current public runtime profile is local/runtime-only. Remote transport
implementation providers are not part of this package surface.

## Contents

```text
include/           Public C ABI headers
native/<platform>/ Prebuilt runtime shared library
cmake/             CMake package config
```

## CMake

```cmake
find_package(CoAkkaRuntimeNativeV2 CONFIG REQUIRED)

add_executable(app main.c)
target_link_libraries(app PRIVATE CoAkkaRuntimeNativeV2::runtime_v2)
```

At runtime, keep the platform native directory on the dynamic loader path, or
copy the matching `libcoakka_runtime_v2` file next to your executable.
