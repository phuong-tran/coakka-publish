# CoAkka Publish

This repository is the public binary-only publish surface for CoAkka artifacts.

## Public Status

Current public runtime generation: `0.1.0+a671b3a`.

| Lane | Public status | Public location |
| --- | --- | --- |
| Logger JVM, Python, Node.js, Go, C#, Rust | public | `logger/{jvm,python,node,go,csharp,rust}/releases/` |
| Logger native C/C++ | public | `logger/native/releases/` |
| Runtime native C ABI | public | `runtime/native/releases/` |
| Runtime JVM, Python, Node.js, Go, C#, Rust | public | `runtime/{jvm,python,node,go,csharp,rust}/releases/` |
| Runtime Spring Boot and Quarkus adapters | public | `maven/` |
| Root runtime headers and native libraries | public | `include/` and `native/` |

The aligned runtime artifact set is anchored on native package generation
`0.1.0+a671b3a`. Do not mix language or framework runtime packages from another
native package generation unless a later release note explicitly declares that
combination compatible.

Consumer-facing downloads are listed in `artifacts/public-artifacts.tsv`.
Integrity metadata is checksum-based for this surface: release manifests,
`SHA256SUMS`, Maven checksum sidecars, and the root artifact manifest. Signature,
SBOM, and attestation files are not part of this release surface yet.

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
- `logger/csharp`
- `logger/rust`
- `logger/native`
- static Maven repo under `maven/`
  - `coakka.logger:coakka-jvm-native-logger`
  - `coakka.v2:coakka-jvm-native-runtime-v2`
  - `coakka.spring:coakka-spring-boot-starter`
  - `coakka.quarkus:coakka-quarkus-extension`
- runtime v2 public `C` ABI files under `include/` and `native/`
- runtime v2 native C ABI archive under `runtime/native/releases/`
- runtime connector packages under `runtime/{jvm,python,node,go,csharp,rust}/releases/`

## Runtime v2 Public Artifacts

Core source snapshot: `a671b3a`

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
- `runtime/native/releases/0.1.0+a671b3a/coakka-runtime-native-v2-0.1.0.tar.gz`

The staged native libraries include the runtime C ABI and the scanner-clean TCP
frame transport candidate used by the public cross-process samples. The
host-facing routing contract remains the same across profiles: targets, route
generations, request/reply, deadletters, and diagnostics stay in the public
runtime contract.

Runtime JVM, Python, Node.js, Go, C#, Rust, Spring Boot, and Quarkus artifacts
are published against the same native package version. Each release directory
has its own manifest and checksums.

Release note:
[`docs/releases/2026-05-09-runtime-a671b3a.md`](docs/releases/2026-05-09-runtime-a671b3a.md)

## Runtime Compatibility Matrix

These artifacts are the currently aligned public runtime set:

| Surface | Artifact | Version | Native package |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/0.1.0+a671b3a/coakka-runtime-native-v2-0.1.0.tar.gz` | `0.1.0+a671b3a` | `0.1.0+a671b3a` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `0.1.1-ga671b3a` | `0.1.0+a671b3a` |
| Python runtime | `coakka_v2_connector` wheel | `0.1.0` | `0.1.0+a671b3a` |
| Node.js runtime | `coakka-v2-connector-node` package | `0.1.0` | `0.1.0+a671b3a` |
| Go runtime | `coakka-v2-connector-go` source package | `0.1.0` | `0.1.0+a671b3a` |
| C# runtime | `CoAkka.Runtime` NuGet package | `0.1.1` | `0.1.0+a671b3a` |
| Rust runtime | `coakka-runtime-rs` spike package | `0.1.0-spike` | `0.1.0+a671b3a` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `0.1.0-ga671b3a` | via JVM runtime `0.1.1-ga671b3a` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `0.1.0-ga671b3a` | via JVM runtime `0.1.1-ga671b3a` |

Do not mix runtime language packages from another native package generation
unless a later release note explicitly declares that combination compatible.

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

The script verifies the root runtime checksums, every logger/runtime release
checksum file, Maven checksum sidecars, runtime JVM release and Maven jars
against the staged native runtime binaries, `artifacts/public-artifacts.tsv`,
and the optional content scanner when the scanner path is provided.

`artifacts/public-artifacts.tsv` is the consumer-facing package manifest. Each
row is status, label, relative path, and SHA256. The public surface gate rejects
rows outside the current public release surface, duplicate labels, duplicate
paths, missing files, and checksum mismatches.

Current integrity metadata is checksum-based: release manifests, `SHA256SUMS`,
Maven checksum sidecars, and `artifacts/public-artifacts.tsv`. Signature,
SBOM, and attestation files are not part of this release surface yet; add them
as explicit release artifacts when the signing flow is introduced.

Before adding a runtime JVM, Python, Node.js, Go, C#, or Rust package release,
run the intake gate on the candidate artifact:

```bash
scripts/verify-runtime-intake-artifact.py \
  --lane python \
  --artifact /path/to/coakka_v2_connector-0.1.0-py3-none-any.whl \
  --expected-native-version 0.1.0+a671b3a
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
- `logger/csharp/releases/`
- `logger/rust/releases/`
- `logger/native/releases/`

The logger lanes keep their own release manifests and checksums.

## Maven Artifacts

The static Maven repository lives under `maven/`. It exposes the logger JVM
artifact, runtime JVM artifact, Spring Boot starter, and Quarkus extension.
