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
  - `coakka.logger:coakka-jvm-native-logger`
- sanitized direct runtime v2 `C` ABI files under `include/` and `native/`
- sanitized runtime v2 native C ABI archive under `runtime/native/releases/`

## Runtime v2 Public Artifacts

Core source snapshot: `63c346e`

Package contents:

- `include/coakka/v2/runtime.h`
- `include/coakka/v2/control.h`
- `include/coakka/v2/client.h`
- `include/coakka/v2/transport.h`
- `include/coakka/v2/utils.h`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `SHA256SUMS`
- `runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz`

The staged native libraries are local/runtime-only public builds. Remote
transport implementation providers remain excluded from this artifact surface.

Runtime language and framework package lanes are paused until their public
package contents are rebuilt against the sanitized runtime surface. Do not
publish or consume runtime JVM, Python, Node.js, Go, C#, Rust, Spring Boot, or
Quarkus artifacts from this repository until those lanes have a release
directory with its own manifest and checksums.

Validation gates run before publishing:

- Linux `aarch64` build and runtime tests
- Linux `x86_64` build and runtime tests
- macOS `aarch64` build and runtime tests
- native dependency scan
- public runtime header render test
- public artifact surface scan

Local repository verification:

```bash
COAKKA_PUBLIC_SURFACE_SCANNER=/path/to/scan_public_artifact_surface.sh \
  scripts/verify-public-surface.sh
```

The script verifies the root runtime checksums, every logger release checksum
file, the native runtime archive checksum, `artifacts/public-artifacts.tsv`,
the absence of paused runtime language/framework package lanes, and the
optional content scanner when the scanner path is provided.

Before adding a runtime JVM, Python, Node.js, Go, C#, or Rust package release,
run the intake gate on the candidate artifact:

```bash
scripts/verify-runtime-intake-artifact.py \
  --lane python \
  --artifact /path/to/coakka_v2_connector-0.1.0-py3-none-any.whl \
  --expected-native-version 0.1.0+63c346e
```

Use the matching `--lane` value for the candidate package. The gate verifies
the bundled native package version, rejects workspace/demo/test paths, rejects
stale native library names, and runs the recursive public surface scanner over
the full artifact contents.

For auxiliary artifacts that intentionally do not embed a native library, such
as a JVM sources jar, add `--allow-no-native`. Main runtime connector packages
must not use that flag.

## Logger Artifacts

Logger packages are published under:

- `logger/jvm/releases/`
- `logger/python/releases/`
- `logger/node/releases/`
- `logger/go/releases/`
- `logger/native/releases/`

The logger lanes keep their own release manifests and checksums.

## Maven Artifacts

The static Maven repository lives under `maven/`. At the moment it only exposes
the logger JVM artifact. Runtime JVM and framework adapter Maven artifacts are
paused until their package contents pass the public artifact surface gate.
