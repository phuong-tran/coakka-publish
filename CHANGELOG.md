# Changelog

This changelog summarizes the public artifact surface. Detailed per-drop notes
remain under [docs/releases/](docs/releases/).

## 2026-08-20

- Published npm, PyPI, and NuGet Runtime `2.5.1` from frozen connector source
  `11c1555`, promoted all eleven non-JVM archive mirrors, and retained exact
  native generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`.
  Registry byte/signature audits and clean consumers pass; Maven Central remains
  on `2.4.1` pending its independent signed release.
- Published and registry-verified `coakka-v2-connector==2.5.0` to PyPI over
  exact native generation
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. The registry wheel is
  byte-identical to candidate `6b56a27`; clean request/reply, five-platform
  payload, File/Stream Lane, and normalized file-scope license gates pass.
- Published and registry-verified Node.js, Bun, and Electron Runtime `2.5.0`
  over exact native generation
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. Registry downloads are
  byte-identical to candidate `6b56a27`; clean Node.js and Bun request/reply
  consumers and the Electron main-process intent path pass on macOS ARM64.
- Published and registry-verified `CoAkka.Runtime==2.5.0` over exact native
  generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. The repository-signed
  download differs from the candidate only by `.signature.p7s`; request/reply
  and route-miss deadletter consumers pass on .NET 8, 9, and 10.
- Published Runtime Go `v1.8.0` and SwiftPM Runtime `v2.5.0` over exact native
  generation `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`. Both annotated
  remote tags, package gates, and clean macOS ARM64 remote-tag consumers pass.
  Maven Central and the promoted native artifact pointer remain on their
  separately listed 2.4.x coordinates.

## 2026-08-18

- Published and registry-verified Node.js, Bun, and Electron Runtime `2.4.1`
  and Logger `1.2.7`, plus Runtime Tauri `2.4.1-source` and Logger Tauri
  `1.2.2-source`, from candidate `7718ce6`. The patch restores macOS deployment
  target 13.0 and declares Node.js 22, Bun 1.2.0, Electron 42, and Rust 1.77.2
  minimum hosts. Exact registry bytes and clean public-coordinate consumers
  pass.

## 2026-08-17

- Published SwiftPM Runtime `v2.4.1` and Logger `v1.2.2`. Both patch releases
  rebuild only their bundled macOS ARM64 dylib with deployment target `13.0`,
  preserve their Swift tools baseline and native generation, and pass public
  CI plus clean consumers cloned from the remote annotated tags.
- Published the Spring Boot starter and Quarkus extension to Maven Central as
  `io.github.phuong-tran.coakka:spring-boot-starter:2.4.1` and
  `io.github.phuong-tran.coakka:quarkus-extension:2.4.1`. Both Java 17 adapter
  bundles use Runtime `2.4.1`, leave framework BOM selection to the
  application, and pass clean public-coordinate request/reply smokes.
- Published and registry-verified `CoAkka.Runtime==2.4.1` and
  `CoAkka.Logger==1.2.3` to NuGet. Both packages target `net8.0`, and their
  exact repository-signed downloads execute on .NET 8, 9, and 10 while
  retaining native generations `2.4.0+c2f53117` and `1.2.1+f50756ebff0d`.
  Every candidate ZIP entry remains byte-identical; NuGet.org adds only
  `.signature.p7s`.
- Published `coakka.logger` to Maven Central as
  `io.github.phuong-tran.coakka:logger:1.2.2`. The signed JVM distribution
  embeds frozen native generation `1.2.1+f50756ebff0d`, supports Java 8 and
  newer JVMs, and links users to the artifact catalog, canonical docs, and
  runnable JVM logger samples.
- Published `coakka.runtime` to Maven Central as
  `io.github.phuong-tran.coakka:runtime:2.4.1`. The signed JVM distribution
  embeds native generation `2.4.0+c2f53117`, supports Java 8 and newer JVMs,
  links its POM to `coakka-publish` and `coakka-samples`, and records the exact
  connector source identity. Runtime and Logger remain independently versioned
  Maven Central components.

## 2026-08-15

- Corrected the newly published native artifact-publisher archives by removing
  local source and object paths retained in macOS debug symbol tables. The
  recorded source snapshots, semantic versions, public C exports, linkage, and
  Linux/Windows payloads are unchanged. Archive manifests and SHA-256 values
  were refreshed; consumers of the earlier checksums must download and verify
  the corrected archives again.

## 2026-08-12

- Reframed the current architecture guidance around CoAkka as a service-mesh
  alternative for runtime traffic. The public docs now connect runtime
  TLS/mTLS, connection strategies, cluster routing, failover, generations, and
  observability directly, while keeping certificate issuance, firewall/CNI,
  public ingress, and observability backends outside the runtime without making
  sidecar proxies a prerequisite.
- Added a startup development guide that begins with an embedded modular
  monolith, supports locally debugged polyglot processes without requiring a
  container build loop, and moves capabilities to independently deployed
  network nodes only when measured ownership, security, isolation, or scaling
  needs justify the operational cost.
- Published the tag-only SwiftPM runtime coordinate
  `github.com/phuong-tran/coakka-runtime-swift@v2.4.0` over native generation
  `2.4.0+c2f53117`. Package readiness and a clean remote-tag request/reply
  consumer pass on macOS ARM64; all five bundled native payloads pass exact
  digest and platform verification.
- Published `coakka-client` and `coakka-runtime-inspect` native archives at
  `2.4.0+c2f53117` for macOS ARM64, Linux ARM64/x86-64, and Windows
  ARM64/x86-64. `coakka-client` command evidence passes on macOS ARM64 and its
  Linux packages pass matching-architecture Docker build/dependency gates.
  Inspect command plus `serve` evidence passes on macOS and Linux. Windows
  packages pass cross-build, PE architecture, dependency, archive, and checksum
  gates without a matching-host execution claim.
- Published and registry-verified `coakka-v2-connector==2.4.0` to PyPI over
  native generation `2.4.0+c2f53117`. The downloaded registry wheel is
  byte-identical to the staged candidate and clean request/reply, matched
  deadletter, hot reload, and local desktop CRUD samples pass.
- Published and registry-verified `CoAkka.Runtime==2.4.0` to NuGet over native
  generation `2.4.0+c2f53117`. NuGet.org adds only `.signature.p7s`; all 16
  staged package entries are byte-identical, the repository signature passes,
  and both a clean registry request/reply/deadletter smoke and live Spring
  Boot-to-C# CRUD exchange succeed.
- Published and registry-verified Node.js, Bun, and Electron runtime packages
  at `2.4.0` over native generation `2.4.0+c2f53117`. Clean registry installs
  pass Node.js and Bun request/reply plus the Electron main-process intent
  smoke.

## 2026-08-11

- Added the independent `runtime-addons/` release family and the unreleased
  SFTP artifact-publisher package contract. The intake gate requires exact
  runtime ABI/feature compatibility, reviewed exports, matching-host platform
  evidence, self-contained native dependencies, safe archive layout, and
  checksums before an addon can enter the public artifact manifest. No SFTP
  addon binary or public install coordinate is claimed by this change.

## 2026-08-10

- Published and registry-verified `coakka-v2-connector==2.3.0` to PyPI over
  native generation `2.3.0+a83ab412`. The registry wheel is byte-identical to
  the staged candidate; clean request/reply, matched deadletter, hot reload,
  local desktop CRUD, and Python File/Stream Lane smokes pass.
- Published and registry-verified `CoAkka.Runtime==2.3.0` to NuGet over native
  generation `2.3.0+a83ab412`. The repository-signed download differs from
  the staged candidate only by `.signature.p7s`; clean request/reply/deadletter
  and C# File/Stream Lane smokes pass.
- Published and registry-verified Node.js, Bun, and Electron runtime packages
  at `2.3.0` over native generation `2.3.0+a83ab412`. The npm lane now exposes
  the official Stream Lane connector surface; its receipt records exact
  tarball digests, registry integrity, platform payloads, and Electron's exact
  Node.js dependency.

## 2026-08-09

- Published Go `v1.5.1` and SwiftPM `v2.1.1` documentation-integrity patches.
  Both keep native generation `2.1.0+60ddf70d`, replace private file-lane links
  with the canonical public contract, and add package regression gates.
- Published and registry-verified `coakka-v2-connector==2.1.0` to PyPI. The
  registry wheel matches the staged SHA-256, contains all five native platform
  payloads, exposes file-lane, and passes readiness plus clean request/reply.
- Published and registry-verified `CoAkka.Runtime==2.1.0` to NuGet with all
  five native RID assets, file-lane, repository-signature verification, and
  packaged request/reply/deadletter execution.
- Published npm runtime patch `2.1.1` for Node.js, Bun, and Electron. The
  package README now links to the canonical public file-lane contract, the
  stale "next runtime" wording is removed, and packaged-doc regression gates
  reject non-public connector repository links.
- Published and registry-verified Node.js, Bun, and Electron runtime packages
  at `2.1.0` over native generation `2.1.0+60ddf70d`. Registry tarballs match
  the staged SHA-256 values byte-for-byte; clean Node.js and Bun installs pass
  package-surface and native request/reply smoke, and Electron resolves its
  exact Node.js `2.1.0` dependency.
- Promoted the complete native/JVM/connector/tool artifact train and public
  file-lane documentation. The public release contains all five native
  libraries and exact Maven, Go, Swift, and connector coordinates.

## 2026-08-08

- Reconciled current package metadata with the registry-verified runtime train:
  npm and PyPI `1.4.6`, NuGet `1.4.7`, Go and Swift `v1.4.1`, and native/JVM/
  source artifacts from `1.4.1+9e02a51d`. SwiftPM current metadata now records
  the five-platform payload and exact `v1.4.1` commit.

## 2026-08-03

- Published and registry-verified the Node.js, Bun, and Electron runtime
  packages at `1.4.6` over native generation `1.4.1+9e02a51d`. The staged
  candidate `37816322` carries the full five-platform native matrix, npm
  serves all three versions under the `latest` dist-tag, and registry checks
  match the staged tarball digests.
- Removed hosted ASan/UBSan execution from `public-surface`. The auditable C11
  harness and sanitizer switches remain available for explicit local or
  evaluator runs; normal artifact CI no longer pays for instrumented builds.
- Added the checksum-manifested public C11 concurrency evidence projection,
  including multi-producer terminal accounting, submit-versus-stop,
  independent runtime lifecycles, quota-gated route-snapshot hot reload, and
  consumer-only TSan controls. Core race claims remain limited to explicit
  exact-source local instrumented runs; published native bytes are unchanged.
  The Windows harness derives an MSVC consumer import library from its public
  export definition without modifying the published runtime DLL.
- Published the native C++ transport configuration contract at a stable public
  URL and made the TLS/mTLS and connection-strategy guides connector-first.
  Both guides now include Kotlin/JVM startup examples and state that full
  runtime connectors expose the same capability-gated contracts without
  direct C use. Root README and Q&A entrypoints now link to those canonical
  guides, and the edge/IoT guide calls out runtime TLS/mTLS for secured device
  and gateway boundaries.
- Published and registry-verified `CoAkka.Runtime==1.4.6` as a gallery-only
  patch over unchanged native runtime generation `1.4.0+2cee86bf`. Its NuGet
  README uses CommonMark for the public logo and an absolute public URL for
  the C# transport guide; runtime behavior and native bytes are unchanged.
- Published and registry-verified `CoAkka.Runtime==1.4.5` on NuGet over
  unchanged native runtime generation `1.4.0+2cee86bf`. A clean registry
  restore loaded the RID native asset and completed request/reply and matched
  deadletter smoke. NuGet repository signing adds only `.signature.p7s`; all
  candidate package entries remain byte-identical.
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
