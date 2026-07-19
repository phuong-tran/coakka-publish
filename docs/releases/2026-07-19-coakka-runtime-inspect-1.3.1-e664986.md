# 2026-07-19 CoAkka Runtime Inspect 1.3.1 e664986

This note records the first public CoAkka Runtime Inspect native archive cut
from source snapshot `e664986`.

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`: the browser can
render runtime facts and generate an equivalent CLI command for the same route
try request shape.

It is not an admin dashboard, schema registry, service discovery server, mTLS
control plane, topology authority, or business schema owner.

| Surface | Artifact root | Version |
| --- | --- | --- |
| Native inspect UI | `runtime-inspect/native/releases/1.3.1+e664986/` | `1.3.1+e664986` |

Native inspect archives in this drop:

- `linux-aarch64`
- `macos-aarch64`

The native archives provide:

- `bin/coakka-runtime-inspect`
- `lib/libcoakka_runtime_v2.so` on Linux
- `lib/libcoakka_runtime_v2.dylib`

Validation covered:

- host-native macOS ARM64 build with embedded `git_commit=e664986`
- Docker Linux ARM64 build with embedded `git_commit=e664986`
- generated archive checksum recorded in `SHA256SUMS`
- generated per-lane `artifacts/public-artifacts.tsv`
- extracted bundle command smoke for `version`, `doctor`, and help
- extracted bundle serve smoke for `/`, `/healthz`, `/api/inspect-info`, and
  `/api/snapshot`

Current limits:

- `/api/snapshot` is a runtime-owned `local-linked-runtime` snapshot from the
  inspect process, not remote runtime read/observe yet.
- `serve --connect host:port` configures the Try Route request path only.
- Linux x86_64 and Windows inspect archives are not published in this drop.
- Business payload schemas, auth policy, service discovery, and mTLS policy
  stay above runtime core.
