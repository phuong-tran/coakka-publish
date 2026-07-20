# 2026-07-20 CoAkka Runtime Inspect 1.3.1 4ce41f19 Linux x86_64

This note records the Linux x86_64 `coakka-runtime-inspect` native archive
refresh cut from source snapshot `4ce41f19`.

## Artifact

| Artifact | Path | Source snapshot |
| --- | --- | --- |
| Native inspect UI, Linux x86_64 | `runtime-inspect/native/releases/1.3.1+4ce41f19/` | `1.3.1+4ce41f19` |

Archive:

- `coakka-runtime-inspect-v2-1.3.1-linux-x86_64.tar.gz`

SHA256:

```text
1733de8d01ed93e36443d502d97b1684784c08ab7bcdb5712ea7b8522ccae92c  coakka-runtime-inspect-v2-1.3.1-linux-x86_64.tar.gz
```

## Verification

The archive was built in a Linux x86_64 Docker builder with the public
self-contained release profile and verified with:

- `check_runtime_native_deps.sh`
- `check_public_runtime_inspect_native_deps.sh`
- `coakka_runtime_inspect_command_smoke`
- `test_coakka_runtime_inspect_serve_smoke.sh`
- `verify_coakka_runtime_inspect_native_bundle.sh`

Direct ELF inspection shows no dynamic protobuf, absl, libuv, libstdc++, or
libgcc runtime dependency. The remaining dynamic edges are the bundled
`libcoakka_runtime_v2.so` plus Linux system libraries.
