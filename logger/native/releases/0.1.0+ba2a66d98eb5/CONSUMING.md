# Consuming CoAkka Logger Native C/C++

Unpack the archive into your dependency directory:

```sh
tar -xzf coakka-logger-native-0.1.0.tar.gz
```

## C ABI

```cmake
set(CMAKE_PREFIX_PATH "/path/to/coakka-logger-native-0.1.0")
find_package(CoAkkaLoggerNative CONFIG REQUIRED)

add_executable(logger_c main.c)
target_link_libraries(logger_c PRIVATE CoAkkaLoggerNative::core)
```

The C ABI is declared by `include/coakka/logger/core.h` and
`include/coakka/logger/utils.h`.

## C++ Wrapper

```cmake
set(CMAKE_PREFIX_PATH "/path/to/coakka-logger-native-0.1.0")
find_package(CoAkkaLoggerNative CONFIG REQUIRED)

add_executable(logger_cpp main.cpp)
target_link_libraries(logger_cpp PRIVATE CoAkkaLoggerNative::native_cpp_connector)
```

The wrapper is intentionally thin. It owns create/start/stop/destroy, exposes
typed log helpers, and keeps `raw()` / `emitRaw()` as explicit escape hatches.

## Platform Selection

The CMake config selects one of these platform directories from the host build:

- `native/linux-aarch64`
- `native/linux-x86_64`
- `native/macos-aarch64`

If your deployment target differs, build the native logger core for that target
and add a matching platform directory before packaging your app.

## Runtime Loader

The executable must be able to find `libcoakka_logger_core` at runtime. During
development, use `DYLD_LIBRARY_PATH` on macOS or `LD_LIBRARY_PATH` on Linux:

```sh
DYLD_LIBRARY_PATH=/path/to/coakka-logger-native-0.1.0/native/macos-aarch64 ./logger_cpp
LD_LIBRARY_PATH=/path/to/coakka-logger-native-0.1.0/native/linux-x86_64 ./logger_cpp
```
