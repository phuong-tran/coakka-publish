# CoAkka Publish

This branch stages the sanitized public CoAkka runtime artifacts before they
are promoted to the public publish channel.

## Runtime v2 Public Artifacts

Core source snapshot: `63c346e`

Package contents:

- `include/coakka/v2/runtime.h`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `SHA256SUMS`

The staged native libraries are local/runtime-only public builds. Remote
transport implementation providers remain excluded from this artifact surface.

Validation gates run before staging:

- Linux `aarch64` build and runtime tests
- Linux `x86_64` build and runtime tests
- native dependency scan
- public runtime header render test
- public artifact surface scan
