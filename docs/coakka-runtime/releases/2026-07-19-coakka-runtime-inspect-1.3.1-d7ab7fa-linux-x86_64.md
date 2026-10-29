# 2026-07-19 CoAkka Runtime Inspect 1.3.1 d7ab7fa Linux x86_64

This note records the public Linux x86_64 `coakka-runtime-inspect` native archive
cut from source snapshot `d7ab7fa`.

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`, not an admin
dashboard, schema registry, service discovery server, mTLS control plane, or
business schema owner.

## Artifact

| Artifact | Path | Source snapshot |
| --- | --- | --- |
| Native inspect UI, Linux x86_64 | `runtime-inspect/native/releases/1.3.1+d7ab7fa/` | `1.3.1+d7ab7fa` |

Archive:

- `coakka-runtime-inspect-v2-1.3.1-linux-x86_64.tar.gz`

Contents:

- `bin/coakka-runtime-inspect`
- `lib/libcoakka_runtime_v2.so`
- `RELEASE.txt`

SHA256:

```text
eaef885f93cd8583f6ce5ca50f6a828b1d6cf779d07c5550086bfce79fbe5860  coakka-runtime-inspect-v2-1.3.1-linux-x86_64.tar.gz
```

## Verification

The archive was built from the native v2 runtime tree and verified inside a
Linux x86_64 Docker build container with:

- `coakka_runtime_inspect_command_smoke`
- `test_coakka_runtime_inspect_serve_smoke.sh`
- `verify_coakka_runtime_inspect_native_bundle.sh`

This refresh includes the current inspect route-try scope note, shared
runtime-surface JSON escaping helper, Copy CLI command-shape smoke, live
deadletter route-try smoke, and Playwright browser-smoke evidence in the source
tree.

## Platform Matrix

Linux x86_64, Linux ARM64, macOS ARM64, Windows x86_64, and Windows ARM64 are
published at `1.3.1+d7ab7fa`.
