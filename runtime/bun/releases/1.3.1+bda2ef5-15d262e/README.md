# coakka-v2-connector-bun

Bun connector package for the CoAkka runtime v2.

This release is built from connector source commit `15d262e` over native runtime
package generation `1.3.1+bda2ef5`.

Current package shape:

- `BunRuntimeHost.start(...)` as the preferred single-process lifecycle
  entrypoint
- `BunConnectorOrchestrator.start(...)` as the compatibility alias
- `BunRuntimeClient` for the lower-level request/reply engine
- shared JavaScript runtime core compiled into a Bun package layout
- bundled native runtime libraries for macOS, Linux, and Windows

## Smoke

```sh
bun add ./coakka-v2-connector-bun-1.3.1.tgz
```

The package embeds native runtime libraries, so normal consumers do not pass a
native library path.
