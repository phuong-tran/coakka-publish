# 2026-07-19 CoAkka Runtime Inspect 1.3.1 5c70234 Windows ARM64

This note records the public Windows ARM64 `coakka-runtime-inspect` native
archive cut from source snapshot `5c70234`.

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`: the browser can
render runtime facts and generate an equivalent CLI command for the same route
try request shape.

It is not an admin dashboard, schema registry, service discovery server, mTLS
control plane, topology authority, or business schema owner.

| Surface | Artifact root | Version |
| --- | --- | --- |
| Native inspect UI, Windows ARM64 | `runtime-inspect/native/releases/1.3.1+5c70234/` | `1.3.1+5c70234` |

Native inspect archives in this follow-on drop:

- `windows-aarch64`

The native archive provides:

- `bin/coakka-runtime-inspect.exe`
- `bin/libcoakka_runtime_v2.dll`

Validation covered:

- Zig Windows ARM64 cross-build with embedded `git_commit=5c70234`
- Windows-target static libuv cache built through the repo-owned cache wrapper
- runtime DLL dependency gate
- inspect executable dependency gate
- generated archive checksum recorded in `SHA256SUMS`
- generated per-lane `artifacts/public-artifacts.tsv`
- staged-prefix smoke on the real UTM `Windows11` guest for `version`,
  `doctor`, `help serve`, and `snapshot --local-route`
- extracted archive smoke on the same UTM `Windows11` guest for the same
  command/snapshot coverage

Current limits:

- `/api/snapshot` is a runtime-owned `local-linked-runtime` snapshot from the
  inspect process, not remote runtime read/observe yet.
- `serve --connect host:port` configures the Try Route request path only.
- Business payload schemas, auth policy, service discovery, and mTLS policy
  stay above runtime core.
