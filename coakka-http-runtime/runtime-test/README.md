# CoAkka HTTP Runtime native test

This is a black-box consumer of the closed CoAkka HTTP Runtime native package.
It includes only `coakka/http/http.h`, links the installed shared library, and
does not read runtime source or private headers.

The test set covers public constants and initializers, invalid arguments,
bounded configuration rejection, lifecycle transitions, one callback-free Core
HTTP request/response exchange with typed terminal observation, live health and
monitor configuration/snapshot/event/policy projection, and 128 concurrent
loopback exchanges across four client threads and four handler workers. The
C++20 test verifies that the same C ABI header is directly consumable from C++.

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/installed/coakka-http-runtime
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

This test surface accompanies a release candidate. A platform is verified only
after its exact artifact has passed this black-box test and binary inspection on
that operating system and architecture.
