# 2026-07-20 CoAkka Runtime Inspect 1.3.1 d7ab7fa Windows

This note records the public Windows `coakka-runtime-inspect` native archive
refresh cut from source snapshot `d7ab7fa`.

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is not a dashboard, schema registry, service discovery
server, mTLS control plane, or business schema owner.

## Artifact

| Lane | Path | Generation |
| --- | --- | --- |
| Native inspect UI, Windows x86_64 | `runtime-inspect/native/releases/1.3.1+d7ab7fa/` | `1.3.1+d7ab7fa` |
| Native inspect UI, Windows ARM64 | `runtime-inspect/native/releases/1.3.1+d7ab7fa/` | `1.3.1+d7ab7fa` |

Platforms:

- `windows-x86_64`
- `windows-aarch64`

The archive contains:

- `bin/coakka-runtime-inspect.exe`
- `bin/libcoakka_runtime_v2.dll`

## Checksums

```text
661a67929c70d706a30a56d005e352e99440f94ba08a97c2e958085e901d5483  coakka-runtime-inspect-v2-1.3.1-windows-aarch64.tar.gz
a09dc984e2c580b047d8fffc8a585f7995e09e86377e48d488a597da80775696  coakka-runtime-inspect-v2-1.3.1-windows-x86_64.tar.gz
```

## Verification

- Zig Windows x86_64 and ARM64 cross-builds with embedded
  `git_commit=d7ab7fa`
- Windows-target static native dependency cache through the repo-owned cache wrapper
- static dependency and architecture gates for both staged prefixes
- staged-prefix smoke on the real UTM `Windows11` guest for `version`,
  `doctor`, `help serve`, and `snapshot --local-route`
- extracted archive smoke on the same UTM `Windows11` guest for the same
  command set

macOS ARM64, Linux x86_64, Linux ARM64, Windows x86_64, and Windows ARM64 are
now all published at `1.3.1+d7ab7fa`.
