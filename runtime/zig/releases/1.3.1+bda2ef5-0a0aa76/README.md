# CoAkka Runtime Zig Connector

Zig connector lane for the CoAkka v2 native runtime ABI.

This lane is source-first and intentionally starts with a small connector
surface:

- dynamically load `libcoakka_runtime_v2`
- read runtime ABI/info
- start one `RuntimeHost`
- apply one local route snapshot
- complete one raw request/reply through the delivered-request lane
- return one route-miss request as a deadletter through the ask-client path
- read runtime stats
- stop and destroy the runtime instance

It does not own retry, transport, or wire policy. Those stay in the native
runtime core and higher-level connector layers.

## Smoke

```sh
COAKKA_RUNTIME_LIB=/path/to/libcoakka_runtime_v2.dylib bash scripts/smoke.sh
```

If `zig` is not installed, the script exits successfully with a skip message.
