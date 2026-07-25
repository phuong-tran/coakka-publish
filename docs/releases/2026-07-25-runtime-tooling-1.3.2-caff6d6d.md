# Runtime Tooling 1.3.2+caff6d6d

This drop retargets the public runtime tools to native runtime generation
`1.3.2+caff6d6d`, matching the runtime native and language-package release
train.

Published artifacts:

- `cli/releases/1.3.2+caff6d6d/`
  - `coakka-client-v2-1.3.2-linux-x86_64.tar.gz`
  - `coakka-client-v2-1.3.2-linux-aarch64.tar.gz`
  - `coakka-client-v2-1.3.2-macos-aarch64.tar.gz`
  - `coakka-client-v2-1.3.2-windows-x86_64.tar.gz`
  - `coakka-client-v2-1.3.2-windows-aarch64.tar.gz`
- `demo/coakka-client/releases/1.3.2+caff6d6d/`
  - `coakka-client-docker-demo-v2-1.3.2-linux-x86_64.tar.gz`
  - `coakka-client-docker-demo-v2-1.3.2-linux-aarch64.tar.gz`
- `runtime-inspect/native/releases/1.3.2+caff6d6d/`
  - `coakka-runtime-inspect-v2-1.3.2-linux-x86_64.tar.gz`
  - `coakka-runtime-inspect-v2-1.3.2-linux-aarch64.tar.gz`
  - `coakka-runtime-inspect-v2-1.3.2-macos-aarch64.tar.gz`
  - `coakka-runtime-inspect-v2-1.3.2-windows-x86_64.tar.gz`
  - `coakka-runtime-inspect-v2-1.3.2-windows-aarch64.tar.gz`

Validation:

- `coakka-client` release verifier passed for Linux x86_64, Linux aarch64,
  macOS aarch64, Windows x86_64, and Windows aarch64.
- `coakka-client` Docker demo release verifier passed for Linux x86_64 and
  Linux aarch64.
- `coakka-runtime-inspect` command and serve smoke passed on Linux x86_64,
  Linux aarch64, and macOS aarch64.
- Windows `coakka-runtime-inspect` bundles were cross-built with Zig and
  checked for runtime/tool dependencies. Runtime smoke is intentionally left to
  a matching Windows host.
- Archive content was audited so Windows bundles contain the Windows `.exe`
  entrypoints and `libcoakka_runtime_v2.dll`, without Linux `.so` or macOS
  `.dylib` payloads.
