# 2026-07-19 CoAkka Runtime Inspect 1.3.1 d7ab7fa macOS ARM64

This note records the public macOS ARM64 `coakka-runtime-inspect` native archive
cut from source snapshot `d7ab7fa`.

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`, not an admin
dashboard, schema registry, service discovery server, mTLS control plane, or
business schema owner.

## Artifact

| Artifact | Path | Source snapshot |
| --- | --- | --- |
| Native inspect UI, macOS ARM64 | `runtime-inspect/native/releases/1.3.1+d7ab7fa/` | `1.3.1+d7ab7fa` |

Archive:

- `coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz`

Contents:

- `bin/coakka-runtime-inspect`
- `lib/libcoakka_runtime_v2.dylib`
- `RELEASE.txt`

SHA256:

```text
5b3bdd7316946b85a284a276aea57c45166c0de7ed9263c191f16e654c19c4ce  coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz
```

## Verification

The archive was built from the native v2 runtime tree and verified on macOS
ARM64 with:

- `coakka_runtime_inspect_command_smoke`
- `test_coakka_runtime_inspect_serve_smoke.sh`
- `verify_coakka_runtime_inspect_native_bundle.sh`

This refresh includes the current inspect route-try scope note, shared
runtime-surface JSON escaping helper, Copy CLI command-shape smoke, live
deadletter route-try smoke, and Playwright browser-smoke evidence in the source
tree.

## Platform Matrix

This is a macOS ARM64 follow-on archive refresh. Linux x86_64/Linux ARM64 remain
published at `1.3.1+e664986`, Windows x86_64 remains published at
`1.3.1+6c63864`, and Windows ARM64 remains published at `1.3.1+5c70234` until
their matching host/guest recuts are completed.
