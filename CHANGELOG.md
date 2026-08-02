# Changelog

This changelog summarizes the public artifact surface. Detailed per-drop notes
remain under [docs/releases/](docs/releases/).

## 2026-08-03

- Published and registry-verified the Node.js, Bun, and Electron runtime
  packages at `1.4.5` over unchanged native generation
  `1.4.0+2cee86bf`. Clean registry installs completed package-boundary checks
  and request/reply or Electron bridge smokes; the aggregate GitHub release
  now also carries the exact three npm tarballs.
- Published and registry-verified `coakka-v2-connector==1.4.5` on PyPI over
  unchanged native runtime generation `1.4.0+2cee86bf`. The PyPI digest matches
  the locally verified wheel, and a clean registry install completed embedded
  native load and request/reply smoke.
- Replaced CoAkka Public Artifact License 1.0 with version 1.1. The new grant
  explicitly permits royalty-free production, internal business, commercial
  application, customer distribution, and SaaS use when CoAkka is an internal
  component. A separate agreement is reserved for Managed CoAkka Services and
  offerings whose primary or substantial value is CoAkka itself.
- Applied the more permissive grant to all official artifacts currently
  distributed from this repository, including runtime `1.4.0`, without
  changing binary bytes, checksums, signing status, or runtime capabilities.

## 2026-08-02

- Bound Linux sanitizer evidence to the released `linux-aarch64` bytes from
  native generation `1.4.0+2cee86bf`, and execute both the public workload and
  four-mode connection-strategy contracts under ASan/UBSan.
- Published native runtime generation `1.4.0+2cee86bf` for Linux ARM64,
  macOS ARM64, and Windows x86-64 with 101 public exports on every platform.
- Published connector artifact generation `1.4.0+2cee86bf-ec4902c` across
  JVM, Node.js, Bun, Electron, Python, Go, C#, Rust, Swift, Tauri, Mojo, and
  Zig, plus matching Spring Boot and Quarkus Maven adapters.
- Published public Go module and SwiftPM releases at `v1.4.0`; npm, PyPI, and
  NuGet retain their last registry-served versions until authenticated 1.4.0
  uploads complete.
- Published `coakka-client` and `coakka-runtime-inspect` 1.4.0 macOS ARM64
  bundles with connected runtime configuration diagnostics; Linux and Windows
  tool artifacts retain their separately recorded prior generations.
- Moved the exact auditable native C11 source projection to root-level
  `runtime-test/`, retained immutable prebuilt evidence archives at their
  historical paths, and kept a compatibility redirect for existing links.
- Added a Linux ARM64 CI lane that verifies the source manifest, runs Clang
  static analysis, and executes the public consumer harness with ASan/UBSan.
- Added the canonical CoAkka guardian logo and brand guide, with registry-safe
  assets shared across package and documentation surfaces.
- Added canonical ecosystem, connection-strategy, TLS/mTLS, support, signing,
  and troubleshooting documentation.
- Documented exact artifact identity by source commit, platform, and SHA-256,
  with publisher signing status reported separately.
- Clarified that Windows, macOS, and Linux remain distribution targets while
  bundled bytes, source compilation, and end-to-end execution are reported as
  separate evidence.
- Replaced public package sequencing with a concise current package-manager
  availability page.
- Included the auditable native C11 evidence source, a SHA-256 source manifest,
  strict static-analysis and consumer sanitizer controls, plus a capability-
  aware connection-strategy contract covering all four modes without making a
  TLS handshake claim.
- Documented connected `coakka-client runtime-info` snapshots and inspect
  transport diagnostics, including capability truth, connection-policy
  provenance, and non-secret TLS/mTLS state.

## 2026-07-31

- Published native evidence runner `1.3.4+dc6ec284` for Linux ARM64/x64,
  macOS ARM64, and Windows ARM64/x64. Linux is the preferred
  deployment-oriented measurement host; Docker, CI, UTM, and other VM results
  are portability evidence only.
- Published and registry-verified `coakka-v2-connector-node@1.3.11` over
  native runtime `1.3.4+dc6ec284`. The release passed clean tarball install,
  native load, and request/reply smoke. Bun and Electron remain at `1.3.9`
  until their package-manager releases are published independently.
- Published and registry-verified `coakka-v2-connector==1.3.6` on PyPI over
  native runtime `1.3.4+dc6ec284`. The release passed clean virtualenv install,
  embedded native load, and request/reply smoke.
- Published and registry-verified `CoAkka.Runtime==1.3.5` on NuGet over native
  runtime `1.3.4+dc6ec284`. The release passed clean project restore, RID native
  load, request/reply, and matched-deadletter smoke.

## 2026-07-25

- Refreshed native `coakka-client`, `coakka-runtime-inspect`, and
  `coakka-client` Docker Linux bundle artifacts to `1.3.1+0da8c2d9` so the
  runtime stop-backpressure hotfix covers the native tooling surface as well
  as the language connector packages.
- Published and registry-verified the npm public metadata and onboarding patch
  release from connector commit `ae6472b`:
  `coakka-v2-connector-node@1.3.7`,
  `coakka-v2-connector-bun@1.3.7`,
  `coakka-v2-connector-electron@1.3.7`,
  `coakka-logger-node@1.2.5`,
  `coakka-logger-bun@1.2.5`, and
  `coakka-logger-electron@1.2.5`.
  These packages keep native runtime generation `1.3.1+0da8c2d9` and logger
  native generation `1.2.1+f50756ebff0d`.

## 2026-07-24

- Documented the public artifact release train in a root changelog.
- Added public compatibility matrix and release checklist documents for the
  artifact repository.
- Documented the planned npm, Go module, crates.io, and apt/deb distribution
  lanes without changing the current artifact or license surface.
- Added an npm package-manager preflight and fixture coverage for the planned
  Node.js, Bun, and Electron runtime/logger lanes without making an npm release
  claim.
- Staged npm-ready candidate tarballs for Node.js, Bun, and Electron runtime
  and logger lanes under `package-manager/npm/candidates/55bbeb7/`.
- Published and registry-verified the npm JavaScript runtime/logger lane:
  `coakka-v2-connector-node@1.3.1`, `coakka-v2-connector-bun@1.3.1`,
  `coakka-v2-connector-electron@1.3.1`, `coakka-logger-node@1.2.1`,
  `coakka-logger-bun@1.2.1`, and `coakka-logger-electron@1.2.1`.
- Added a first-reader `New To CoAkka` entrypoint that explains the runtime,
  logger, npm packages, and the relationship between `coakka-publish` and
  `coakka-samples`.
- Staged npm onboarding patch release candidates from connector commit
  `e5d3342`: runtime packages `1.3.2` and logger packages `1.2.2`, with the
  same native runtime/logger generations as the previous npm release.
- Published and registry-verified the npm onboarding patch release:
  `coakka-v2-connector-node@1.3.2`, `coakka-v2-connector-bun@1.3.2`,
  `coakka-v2-connector-electron@1.3.2`, `coakka-logger-node@1.2.2`,
  `coakka-logger-bun@1.2.2`, and `coakka-logger-electron@1.2.2`.
- Staged runtime npm metadata patch candidates from connector commit
  `b6a1bf2`: runtime packages `1.3.3`, with the same native runtime
  generation `1.3.1+bda2ef5`.
- Published and registry-verified the runtime npm metadata patch release:
  `coakka-v2-connector-node@1.3.3`,
  `coakka-v2-connector-bun@1.3.3`, and
  `coakka-v2-connector-electron@1.3.3`.
- Staged npm README first-run smoke patch candidates from connector commit
  `f3e25ec`: runtime packages `1.3.4` and logger packages `1.2.3`, with the
  same native runtime/logger generations as the current npm release.
- Published and registry-verified the npm README first-run smoke patch release:
  `coakka-v2-connector-node@1.3.4`,
  `coakka-v2-connector-bun@1.3.4`,
  `coakka-v2-connector-electron@1.3.4`,
  `coakka-logger-node@1.2.3`,
  `coakka-logger-bun@1.2.3`, and
  `coakka-logger-electron@1.2.3`.
- Staged npm package metadata links patch candidates from connector commit
  `e2c2442`: runtime packages `1.3.5` and logger packages `1.2.4`, with the
  same native runtime/logger generations as the current npm release.
- Published and registry-verified the npm package metadata links patch release:
  `coakka-v2-connector-node@1.3.5`,
  `coakka-v2-connector-bun@1.3.5`,
  `coakka-v2-connector-electron@1.3.5`,
  `coakka-logger-node@1.2.4`,
  `coakka-logger-bun@1.2.4`, and
  `coakka-logger-electron@1.2.4`.

## 2026-07-23

- Added public Runtime Bun connector packages for the current `1.3.1` train.
- Added public Runtime Tauri intent source packages for the current `1.3.1`
  train.
- Added public Runtime Electron connector packages for the current `1.3.1`
  train.
- Added public Logger Bun connector packages for the current `1.2.1` logger
  train.
- Added public Logger Tauri source and Electron connector packages for the
  current `1.2.1` logger train.
- Tightened the public boundary checks for Bun, Tauri, and Electron packages so
  first-run consumers do not need extra native implementation packages.
- See:
  [Runtime Bun and Tauri](docs/releases/2026-07-23-runtime-bun-tauri-1.3.1-247df1b.md),
  [Runtime Electron](docs/releases/2026-07-23-runtime-electron-1.3.1-4e0cab0.md),
  [Logger Bun](docs/releases/2026-07-23-logger-bun-1.2.1-6fdcc69.md),
  and
  [Logger Tauri/Electron](docs/releases/2026-07-23-logger-tauri-electron-1.2.1-3e8a6ae.md).

## 2026-07-20

- Expanded `coakka-runtime-inspect` public archives across Linux, macOS, and
  Windows lanes.
- Added the public Docker Hub sample image for `coakka-runtime-inspect`.
- Refreshed inspect release metadata and self-contained package checks.
- See:
  [Inspect Linux x86_64](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-x86_64.md),
  [Inspect Linux ARM64](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-aarch64.md),
  [Inspect Windows](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-d7ab7fa-windows.md),
  and
  [Inspect Docker Hub](docs/releases/2026-07-20-coakka-runtime-inspect-dockerhub-1.3.1-4ce41f19.md).

## 2026-07-18

- Published the public `1.3.1` runtime artifact train.
- Published `coakka-client` `1.3.1` native CLI archives and Docker Linux
  bundles.
- Refreshed JVM, framework adapter, and non-JVM runtime connector packages over
  the current native runtime generation.
- Documented the current public release boundary and GitHub Release asset
  layout.
- See:
  [Runtime native 1.3.1](docs/releases/2026-07-18-runtime-native-1.3.1-bda2ef5.md),
  [Runtime JVM 1.3.1](docs/releases/2026-07-18-runtime-jvm-1.3.1-0a0aa76.md),
  [Runtime adapters 1.3.1](docs/releases/2026-07-18-runtime-jvm-adapters-1.3.1-0a0aa76.md),
  [Runtime non-JVM 1.3.1](docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md),
  [coakka-client 1.3.1](docs/releases/2026-07-18-coakka-client-1.3.1-2215b0f.md),
  and
  [Public release boundary 1.3.1](docs/releases/2026-07-18-public-release-boundary-1.3.1.md).

## 2026-07-06

- Published the Runtime `1.2.1` family refresh.
- Published Logger `1.2.1` with Windows parity.
- Added community repository templates and contribution/support entrypoints.
- See:
  [Runtime native 1.2.1](docs/releases/2026-07-06-runtime-native-1.2.1-abde383.md),
  [Runtime JVM 1.2.1](docs/releases/2026-07-06-runtime-jvm-1.2.1-fa29f94.md),
  [Runtime adapters 1.2.1](docs/releases/2026-07-06-runtime-jvm-adapters-1.2.1-fa29f94.md),
  [Runtime non-JVM 1.2.1](docs/releases/2026-07-06-runtime-non-jvm-1.2.1-fa29f94.md),
  and
  [Logger 1.2.1](docs/releases/2026-07-06-logger-1.2.1-8264bba.md).

## 2026-07-05

- Published refreshed JVM and non-JVM runtime connector trains.
- Published Logger Mojo and Zig source lanes.
- Hardened native publish verification.
- See:
  [Logger Mojo/Zig source](docs/releases/2026-07-05-logger-zig-mojo-source-1107154.md),
  [Runtime JVM refresh](docs/releases/2026-07-05-runtime-jvm-c124a9e-da30a95.md),
  and
  [Runtime non-JVM refresh](docs/releases/2026-07-05-runtime-non-jvm-c124a9e-66ebe58.md).

## 2026-05

- Opened the public artifact channel and standardized the publish surface.
- Added public surface verification, manifest checks, intake checks, checksum
  sidecars, and GitHub Actions validation.
- Published early runtime native, JVM, Python, Node.js, Go, C#, Rust, Mojo, and
  Zig artifact lanes.
- Published early logger JVM, Python, Node.js, Go, C#, Rust, native, Mojo, and
  Zig artifact lanes.
- Added connector UX documentation and framework adapter artifacts.
