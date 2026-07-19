# 2026-07-19 CoAkka Runtime Inspect 1.3.1 6c63864 Windows x86_64

This note records the first public Windows x86_64
`coakka-runtime-inspect` native archive cut from source snapshot `6c63864`.

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`: the browser can
render runtime facts and generate an equivalent CLI command for the same route
try request shape.

It is not an admin dashboard, schema registry, service discovery server, mTLS
control plane, topology authority, or business schema owner.

| Surface | Artifact root | Version |
| --- | --- | --- |
| Native inspect UI, Windows x86_64 | `runtime-inspect/native/releases/1.3.1+6c63864/` | `1.3.1+6c63864` |

Native inspect archives in this follow-on drop:

- `windows-x86_64`

The native archive provides:

- `bin/coakka-runtime-inspect.exe`
- `bin/libcoakka_runtime_v2.dll`

Validation covered:

- Zig Windows x86_64 cross-build with embedded `git_commit=6c63864`
- Windows-target static libuv cache built through the repo-owned cache wrapper
- runtime DLL dependency gate
- inspect executable dependency gate
- generated archive checksum recorded in `SHA256SUMS`
- generated per-lane `artifacts/public-artifacts.tsv`
- extracted archive smoke on the real UTM `Windows11` guest for `version`,
  `doctor`, `help serve`, and `snapshot --local-route`

Current limits:

- `/api/snapshot` is a runtime-owned `local-linked-runtime` snapshot from the
  inspect process, not remote runtime read/observe yet.
- `serve --connect host:port` configures the Try Route request path only.
- Windows ARM64 inspect archive is not published in this drop.
- Business payload schemas, auth policy, service discovery, and mTLS policy
  stay above runtime core.
