# CoAkka Runtime Mojo Connector

Mojo connector lane for the CoAkka v2 native runtime ABI.

This source-first lane is intentionally narrower than JVM/Python/Node/Go, but
it is now more than an ABI probe:

- load one sample-local connector shim through Mojo FFI
- start one runtime host through the native C ABI
- apply one local route snapshot
- complete one raw request/reply through the delivered-request lane
- return one route-miss request as a deadletter through the ask-client path
- read runtime stats
- stop and destroy the runtime instance

Mojo's FFI surface is still moving, so this lane keeps lifecycle/control logic
behind a C shim until direct function binding is stable enough for a fuller
host-native connector. Treat the exported C shim function as package smoke
infrastructure, not an application API. The smoke script skips when `mojo` is
not installed.

## Smoke

```sh
COAKKA_RUNTIME_LIB=/path/to/libcoakka_runtime_v2.dylib bash scripts/smoke.sh
```
