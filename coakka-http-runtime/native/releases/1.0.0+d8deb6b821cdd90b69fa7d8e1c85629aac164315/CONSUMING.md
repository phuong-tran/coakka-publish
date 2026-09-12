# Consuming CoAkka HTTP Core Native

Point CMake at exactly one target tree:

```sh
cmake -S app -B build \
  -DCMAKE_PREFIX_PATH=/path/to/release/linux-x86_64
cmake --build build
```

```cmake
find_package(CoAkkaHttp 1.0.0 EXACT CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CoAkkaHttp::runtime)
```

Application source includes only:

```c
#include <coakka/http/http.h>
```

On Windows, place `coakka_http_runtime.dll` beside the executable or in an
application-controlled DLL search directory. On Linux/macOS, install or bundle
the matching shared library according to the application's loader policy.

Core copies successful configuration and command input. Returned event,
WebSocket and outbound-terminal views remain native leases until their exact
release function is called. Follow the header's one-reader-per-lane and
stop/destroy rules; never destroy while a call or lease is outstanding.
