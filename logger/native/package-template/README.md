# CoAkka Logger Native C/C++

This package contains the public logger C ABI, a small C++ convenience wrapper,
and prebuilt native logger cores for:

- `linux-aarch64`
- `linux-x86_64`
- `macos-aarch64`

The host application owns configuration and lifecycle. The native logger core
only receives explicit API calls from the host process.

## Contents

```text
include/                     C ABI and C++ wrapper headers
src/native_cpp/Connector.cpp  C++ wrapper implementation
native/<platform>/            Prebuilt logger core shared library
cmake/                        CMake package config
examples/                     C and C++ smoke examples
```

## CMake

```cmake
find_package(CoAkkaLoggerNative CONFIG REQUIRED)

add_executable(app main.c)
target_link_libraries(app PRIVATE CoAkkaLoggerNative::core)
```

For the C++ wrapper:

```cmake
find_package(CoAkkaLoggerNative CONFIG REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE CoAkkaLoggerNative::native_cpp_connector)
```

At runtime, keep the platform native directory on the dynamic loader path, or
copy the matching `libcoakka_logger_core` file next to your executable.
