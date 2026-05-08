# CoAkka Publish

This repository is the public binary-only publish surface for CoAkka artifacts.

Repository rules:

- keep this as a plain artifact repository, not a source-build repository
- store released drops under product lane, then language lane
- keep `manifest.json`, `SHA256SUMS`, and one short release `README.md` with
  each drop
- keep public artifacts aligned with
  [`docs/public-artifact-contract.md`](docs/public-artifact-contract.md)

Current published lanes:

- `logger/jvm`
- `logger/python`
- `logger/node`
- `logger/go`
- `logger/native`
- static Maven repo under `maven/`
  - `coakka.spring:coakka-spring-boot-starter`
  - `coakka.quarkus:coakka-quarkus-extension`
- sanitized direct runtime v2 `C` ABI files under `include/` and `native/`

## Runtime v2 Public Artifacts

Core source snapshot: `63c346e`

Package contents:

- `include/coakka/v2/runtime.h`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `SHA256SUMS`

The staged native libraries are local/runtime-only public builds. Remote
transport implementation providers remain excluded from this artifact surface.

Validation gates run before publishing:

- Linux `aarch64` build and runtime tests
- Linux `x86_64` build and runtime tests
- macOS `aarch64` build and runtime tests
- native dependency scan
- public runtime header render test
- public artifact surface scan

## Logger Artifacts

Logger packages are published under:

- `logger/jvm/releases/`
- `logger/python/releases/`
- `logger/node/releases/`
- `logger/go/releases/`
- `logger/native/releases/`

The logger lanes keep their own release manifests and checksums.

## Maven Artifacts

The static Maven repository lives under `maven/`. It includes published JVM
runtime, logger, Spring Boot starter, and Quarkus extension/starter artifacts
with Maven metadata and per-file checksums.
