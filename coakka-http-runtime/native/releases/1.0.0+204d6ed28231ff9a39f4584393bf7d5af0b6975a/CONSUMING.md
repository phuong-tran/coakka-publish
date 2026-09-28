# Consuming The Native SDK

Point CMake at one target tree:

```sh
cmake -S app -B build -DCMAKE_PREFIX_PATH=/path/to/release/linux-x86_64
cmake --build build
```

```cmake
find_package(CoAkkaHttp 1.0.0 EXACT CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CoAkkaHttp::runtime)
```

Application source includes only `<coakka/http/http.h>`. On Windows, deploy
`coakka_http_runtime.dll` beside the executable or in an application-owned DLL
directory. Follow the header's lease, one-reader-per-lane and stop/destroy
rules; never destroy while a call or lease is outstanding.

