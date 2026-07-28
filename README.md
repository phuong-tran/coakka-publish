# CoAkka Publish

[![public-surface](https://github.com/phuong-tran/coakka-publish/actions/workflows/public-surface.yml/badge.svg)](https://github.com/phuong-tran/coakka-publish/actions/workflows/public-surface.yml)

Contribution guide: [CONTRIBUTING.md](CONTRIBUTING.md)
Support: [SUPPORT.md](SUPPORT.md)
Changelog: [CHANGELOG.md](CHANGELOG.md)
New to CoAkka: [docs/new-to-coakka.md](docs/new-to-coakka.md)
CoAkka story: [docs/coakka-story.md](docs/coakka-story.md)
Runtime client: [docs/coakka-runtime-client.md](docs/coakka-runtime-client.md)
Runtime inspect: [docs/coakka-runtime-inspect.md](docs/coakka-runtime-inspect.md)
Compatibility matrix: [docs/compatibility-matrix.md](docs/compatibility-matrix.md)
Package manager roadmap: [docs/package-manager-roadmap.md](docs/package-manager-roadmap.md)
How it works: [docs/how-it-works.md](docs/how-it-works.md)
Spring Boot: [docs/coakka-spring-boot.md](docs/coakka-spring-boot.md)
Quarkus: [docs/coakka-quarkus.md](docs/coakka-quarkus.md)
Runtime glossary: [docs/runtime-glossary.md](docs/runtime-glossary.md)
Runtime integration guide: [docs/runtime-integration-guide.md](docs/runtime-integration-guide.md)
Incremental adoption: [docs/incremental-adoption.md](docs/incremental-adoption.md)
Production readiness: [docs/production-readiness.md](docs/production-readiness.md)
Questions and answers: [docs/qna.md](docs/qna.md)

This repository is the public binary-only publish surface for CoAkka artifacts.
If you are new to CoAkka, start with
[New To CoAkka](docs/new-to-coakka.md), then use
[`coakka-samples`](https://github.com/phuong-tran/coakka-samples) to run
examples.

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. It helps an app route work by target name, handle request/reply,
deadletters, bounded queues, diagnostics, and native-backed logging without
turning every internal boundary into another hand-written HTTP endpoint.

## Start Here

| Question | Short answer |
| --- | --- |
| Problem | Internal application work often becomes fake backend HTTP, spreading one contract across URLs, clients, retries, timeout mapping, status mapping, and logs. |
| What CoAkka is | A runtime boundary for application capabilities: callers ask a typed target, route snapshots decide ownership, and replies/deadletters carry runtime diagnostics. |
| What this repo is | The public artifact surface: packages, native archives, manifests, checksums, compatibility matrix, and release notes. |
| What it is not | Not the runnable sample repo and not a source-build repository. Use `coakka-samples` to run examples. |
| How to adopt | Pick one painful internal boundary, keep legacy HTTP/gRPC edges intact, and migrate gradually where runtime targets make ownership clearer. |

Public repository map:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |
| `coakka-samples` | Runnable examples that show how an app uses the runtime and logger packages. | https://github.com/phuong-tran/coakka-samples |
| `coakka-runtime-go` | Public Go module for CoAkka Runtime. | https://github.com/phuong-tran/coakka-runtime-go |
| `coakka-logger-go` | Public Go module for CoAkka Logger. | https://github.com/phuong-tran/coakka-logger-go |
| `coakka-runtime-swift` | Public SwiftPM package for CoAkka Runtime on macOS ARM64. | https://github.com/phuong-tran/coakka-runtime-swift |
| `coakka-logger-swift` | Public SwiftPM package for CoAkka Logger on macOS ARM64. | https://github.com/phuong-tran/coakka-logger-swift |

Current package-manager entrypoints:

| Channel | Runtime package | Logger package |
| --- | --- | --- |
| NuGet | [`CoAkka.Runtime` 1.3.3](https://www.nuget.org/packages/CoAkka.Runtime/1.3.3) | [`CoAkka.Logger` 1.2.2](https://www.nuget.org/packages/CoAkka.Logger/1.2.2) |
| npm | [`coakka-v2-connector-node` 1.3.9](https://www.npmjs.com/package/coakka-v2-connector-node/v/1.3.9) | [`coakka-logger-node` 1.2.6](https://www.npmjs.com/package/coakka-logger-node/v/1.2.6) |
| PyPI | [`coakka-v2-connector` 1.3.4](https://pypi.org/project/coakka-v2-connector/1.3.4/) | [`coakka-logger` 1.2.2](https://pypi.org/project/coakka-logger/1.2.2/) |
| Go modules | [`coakka-runtime-go` v1.3.10](https://pkg.go.dev/github.com/phuong-tran/coakka-runtime-go@v1.3.10) | [`coakka-logger-go` v1.2.5](https://pkg.go.dev/github.com/phuong-tran/coakka-logger-go@v1.2.5) |
| SwiftPM | [`coakka-runtime-swift` v1.3.2](https://github.com/phuong-tran/coakka-runtime-swift/releases/tag/v1.3.2) | [`coakka-logger-swift` v1.2.1](https://github.com/phuong-tran/coakka-logger-swift/releases/tag/v1.2.1) |

Public docs:
[The CoAkka Story](docs/coakka-story.md),
[How It Works](docs/how-it-works.md),
[CoAkka Spring Boot](docs/coakka-spring-boot.md),
[CoAkka Quarkus](docs/coakka-quarkus.md),
[CoAkka Runtime Client](docs/coakka-runtime-client.md),
[CoAkka Runtime Inspect](docs/coakka-runtime-inspect.md),
[Runtime Glossary](docs/runtime-glossary.md),
[Runtime Message And Routing Model](docs/runtime-message-and-routing-model.md),
[Runtime Integration Guide](docs/runtime-integration-guide.md),
[Incremental Adoption](docs/incremental-adoption.md),
[Cluster Routing](docs/runtime-cluster-routing.md),
[Containerized Runtime](docs/containerized-runtime.md),
[Production Readiness](docs/production-readiness.md),
[Production Evidence](docs/production-evidence.md),
[Questions And Answers](docs/qna.md),
and
[Repository Boundaries](docs/repository-boundaries.md).

Latest GitHub Release page with attached public artifacts:
[CoAkka Public Artifacts 1.3.1](https://github.com/phuong-tran/coakka-publish/releases/tag/coakka-public-artifacts-v1.3.1).

The current artifact mirror is newer than that GitHub Release page. Use
[`artifacts/public-artifacts.tsv`](artifacts/public-artifacts.tsv) and
[`docs/compatibility-matrix.md`](docs/compatibility-matrix.md) as the current
public artifact index.

Current runtime native release note:
[2026-07-25 runtime native 1.3.2 caff6d6d](docs/releases/2026-07-25-runtime-native-1.3.2-caff6d6d.md).

Current coakka-client release note:
[2026-07-25 runtime tooling 1.3.2 caff6d6d](docs/releases/2026-07-25-runtime-tooling-1.3.2-caff6d6d.md).

Current coakka-runtime-inspect release notes:
[2026-07-25 runtime tooling 1.3.2 caff6d6d](docs/releases/2026-07-25-runtime-tooling-1.3.2-caff6d6d.md).
Last documented coakka-runtime-inspect Docker Hub release note:
[2026-07-25 runtime tooling stop backpressure hotfix 0da8c2d9](docs/releases/2026-07-25-runtime-tooling-stop-backpressure-hotfix-0da8c2d9.md).

Latest GitHub Release boundary note:
[2026-07-18 public release boundary 1.3.1](docs/releases/2026-07-18-public-release-boundary-1.3.1.md).

Current language connector release notes:
[2026-07-26 Go module native payload cleanup](docs/releases/2026-07-26-go-module-native-payload-cleanup.md),
[2026-07-28 C# runtime NuGet 1.3.3](docs/releases/2026-07-28-csharp-runtime-nuget-1.3.3.md),
[2026-07-28 C# logger NuGet 1.2.2](docs/releases/2026-07-28-csharp-logger-nuget-1.2.2.md),
[2026-07-25 runtime language packages 1.3.2 caff6d6d 6d5ea58](docs/releases/2026-07-25-runtime-language-packages-1.3.2-caff6d6d-6d5ea58.md),
[2026-07-25 npm runtime package b46f705](docs/releases/2026-07-25-npm-runtime-native-bridge-b46f705.md),
[2026-07-23 logger Tauri and Electron 1.2.1 refresh 3e8a6ae](docs/releases/2026-07-23-logger-tauri-electron-1.2.1-3e8a6ae.md),
[2026-07-23 logger Bun 1.2.1 refresh 6fdcc69](docs/releases/2026-07-23-logger-bun-1.2.1-6fdcc69.md),
[2026-07-06 logger 1.2.1 Windows parity 8264bba](docs/releases/2026-07-06-logger-1.2.1-8264bba.md),
and
[2026-07-05 logger Mojo and Zig source refresh 1107154](docs/releases/2026-07-05-logger-zig-mojo-source-1107154.md).

Historical runtime connector release notes:
[2026-07-06 runtime JVM 1.2.1 refresh fa29f94](docs/releases/2026-07-06-runtime-jvm-1.2.1-fa29f94.md),
[2026-07-06 runtime JVM adapters 1.2.1 refresh fa29f94](docs/releases/2026-07-06-runtime-jvm-adapters-1.2.1-fa29f94.md),
[2026-07-06 runtime non-JVM 1.2.1 refresh fa29f94](docs/releases/2026-07-06-runtime-non-jvm-1.2.1-fa29f94.md),
[2026-07-06 runtime JVM adapters b8ecfae refresh 2d085e5](docs/releases/2026-07-06-runtime-jvm-adapters-b8ecfae-2d085e5.md),
[2026-07-06 runtime JVM b8ecfae refresh 2d085e5](docs/releases/2026-07-06-runtime-jvm-b8ecfae-2d085e5.md),
[2026-07-06 runtime non-JVM b8ecfae refresh 2d085e5](docs/releases/2026-07-06-runtime-non-jvm-b8ecfae-2d085e5.md),
[2026-05-23 runtime JVM adapters c124a9e](docs/releases/2026-05-23-runtime-jvm-adapters-c124a9e-1c2694b.md),
[2026-05-23 runtime JVM c124a9e rebundle](docs/releases/2026-05-23-runtime-jvm-c124a9e-2bab9ee.md)
and
[2026-05-21 runtime connector 5ab812f](docs/releases/2026-05-21-runtime-connector-5ab812f.md).

Current source connector release note:
[2026-07-25 runtime language packages 1.3.2 caff6d6d 6d5ea58](docs/releases/2026-07-25-runtime-language-packages-1.3.2-caff6d6d-6d5ea58.md).

## License And Trademark

This repository distributes CoAkka artifacts under the
[CoAkka Public Artifact Preview License 1.0](LICENSE.md), unless a specific
release artifact includes different license terms. This is not an OSI-approved
open source license.

The repository is intended for developer evaluation, official sample
execution, CI, integration bring-up, and integration testing, including company
evaluations inside commercial organizations. Production means environments
serving live end-user traffic, live customer data, live operational data,
revenue-generating workloads, or customer-facing workloads outside
development, test, CI, sample execution, or proof-of-concept evaluation.
Production use, hosted service use, customer distribution, and paid
redistribution require explicit release terms or a separate written agreement.

Official CoAkka sample images published by the project or repository owner,
including the current `docker.io/gabrielgun1983/*` sample namespace and any
future official CoAkka container namespace, may bundle unmodified artifacts
solely to run official samples. Users may pull, cache internally, and run those
official sample images for local development, CI, test, sample execution,
proof-of-concept integration, and evaluation. This does not grant third
parties rights to create derivative production images, hosted services, paid
support packages, appliances, or cloud marketplace offerings.

The separate `coakka-samples` repository contains sample code and documentation
under its own license. That sample license does not change the terms for the
runtime binaries, connector packages, Maven artifacts, or other released
artifacts distributed here.

For production, hosted service, customer distribution, paid redistribution, or
other commercial rights, use the private contact path in [SUPPORT.md](SUPPORT.md).

The CoAkka name and `coakka` package, artifact, and image prefixes identify
the official project surface. See [TRADEMARKS.md](TRADEMARKS.md) before using
the name for forks, derived runtimes, hosted services, mirrors, or product
branding.

## Public Status

Current public logger native generation: `1.2.1+f50756ebff0d`.
Current public logger Bun connector generation: `1.2.1+f50756ebff0d-6fdcc69`.
Current public logger Tauri/Electron connector generation:
`1.2.1+f50756ebff0d-3e8a6ae`.
Current public logger Mojo/Zig source connector generation:
`1.2.1+f50756ebff0d-8264bba`.
Current public native runtime generation: `1.3.2+caff6d6d`.
Current public JVM connector generation: `1.3.2+caff6d6d-6d5ea58`.
Current public Spring Boot and Quarkus adapter generation:
`1.3.2-gcaff6d6d-6d5ea58`.
Current public Python, Go, C#, and Rust connector generation:
`1.3.2+caff6d6d-6d5ea58`.
Current public Node.js, Bun, and Electron npm runtime packages:
`coakka-v2-connector-{node,bun,electron}@1.3.9`.
Current public C# runtime NuGet package: `CoAkka.Runtime@1.3.3`.
Current public C# logger NuGet package: `CoAkka.Logger@1.2.2`.
Current public Mojo/Zig source connector generation:
`1.3.2+caff6d6d-6d5ea58`.
Current public Tauri intent source connector generation:
`1.3.2+caff6d6d-6d5ea58`.
Current public coakka-client generation: `1.3.2+caff6d6d`.
Current public coakka-client Docker Linux bundle generation: `1.3.2+caff6d6d`.
Current public coakka-runtime-inspect native generation: `1.3.2+caff6d6d`.
Docker Hub image tags are not part of the current release ledger yet; the
GitHub demo bundles above are the captured public sample artifacts.

| Lane | Public status | Public location |
| --- | --- | --- |
| Logger JVM, Python, Node.js, Bun, Electron, Go, C#, Rust | public | `logger/{jvm,python,node,bun,electron,go,csharp,rust}/releases/` |
| Logger Swift | public SwiftPM | `https://github.com/phuong-tran/coakka-logger-swift`, tag `v1.2.1` |
| Logger Tauri, Mojo, and Zig source connector lanes | public source packages | `logger/{tauri,mojo,zig}/releases/` |
| Logger native C/C++ | public | `logger/native/releases/` |
| Runtime native C ABI | public | `runtime/native/releases/` |
| Runtime JVM, Python, Node.js, Bun, Electron, Go, C#, Rust | public | `runtime/{jvm,python,node,bun,electron,go,csharp,rust}/releases/` |
| Runtime Swift | public SwiftPM | `https://github.com/phuong-tran/coakka-runtime-swift`, tag `v1.3.2` |
| Runtime Mojo, Zig, and Tauri source connector lanes | public source packages | `runtime/{mojo,zig,tauri}/releases/` |
| Runtime Spring Boot and Quarkus adapters | public | `maven/` |
| Root runtime headers and native libraries | public | `include/` and `native/` |
| coakka-client native CLI | public | `coakka-tools/coakka-client/releases/` |
| coakka-client Docker Linux bundle | public Linux bundles | `coakka-tools/coakka-client/docker-demo/releases/` |
| coakka-runtime-inspect native UI | public macOS ARM64, Linux x86_64/ARM64, and Windows x86_64/ARM64 | `coakka-tools/coakka-runtime-inspect/releases/` |
| Docker Hub sample images | not captured in the current release ledger | see per-release Docker Hub notes |

SwiftPM package-manager releases:

| Lane | Coordinate | Native generation |
| --- | --- | --- |
| Swift runtime connector | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `1.3.2` | runtime native `1.3.2+caff6d6d` |
| Swift logger connector | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1` | logger native `1.2.1+f50756ebff0d` |

The SwiftPM lane is source-first and macOS ARM64. Its release manifest is
tracked under `package-manager/swiftpm/current.json`.

NuGet package-manager releases:

| Lane | Coordinate | Native generation |
| --- | --- | --- |
| C# runtime connector | `CoAkka.Runtime`, exact `1.3.3` | runtime native `1.3.2+caff6d6d` |
| C# logger connector | `CoAkka.Logger`, exact `1.2.2` | logger native `1.2.1+f50756ebff0d` |

The NuGet lane is binary package-manager distribution for .NET consumers.
Its release manifest is tracked under
`package-manager/nuget/current.json`.

The current root native runtime package is `1.3.2+caff6d6d`. The published JVM
runtime jar is refreshed over that native package as
`1.3.2-gcaff6d6d-6d5ea58`. Published Spring Boot and Quarkus adapters are
rebuilt as `1.3.2-gcaff6d6d-6d5ea58` over that JVM runtime. Python, Go, C#,
Rust, Mojo, Zig, and Tauri publish against the same `1.3.2+caff6d6d` native
package through connector source commit `6d5ea58`. Node.js, Bun, and Electron
npm runtime packages publish as `1.3.9` over connector source commit `b46f705`
and the same native runtime generation.
Do not mix language or framework runtime packages from another native package
generation unless a release note explicitly declares that combination
compatible.

For development and integration work, macOS and Windows are both supported host
surfaces in the current train. The current published package surface bundles
macOS, Linux, and Windows runtime natives. Linux remains the normal deployment
path for release, soak, and operator-readiness claims.

Consumer-facing downloads are attached to the GitHub Release page and listed
in `artifacts/public-artifacts.tsv`. Integrity metadata is checksum-based for
this surface: release manifests, `SHA256SUMS`, Maven checksum sidecars, and
the root artifact manifest. Signature, SBOM, and attestation files are not part
of this release surface yet. The manifest also keeps explicit provenance
comments for public Mojo/Zig source-package lanes so the connector source
commit is visible at the package list boundary.

The npm package-manager lane is current for JavaScript runtime/logger packages.
The Python runtime package `coakka-v2-connector==1.3.4` and logger package
`coakka-logger==1.2.2` are published on PyPI. Superseded PyPI versions
`coakka-v2-connector==1.3.2`, `coakka-v2-connector==1.3.3`, and
`coakka-logger==1.2.1` are yanked so ordinary installs resolve to the current
patch versions. The Go runtime package
`github.com/phuong-tran/coakka-runtime-go@v1.3.10` and logger package
`github.com/phuong-tran/coakka-logger-go@v1.2.5` are published as public Go
modules. SwiftPM packages for Swift runtime/logger are published as public
GitHub tag coordinates. NuGet packages for C# runtime/logger are published as
public package coordinates. crates.io and apt/deb remain planned distribution
surfaces. The intended order and release gates are tracked in
[`docs/package-manager-roadmap.md`](docs/package-manager-roadmap.md).

The current public logger family keeps native package generation
`1.2.1+f50756ebff0d`. JVM, Python, Node.js, Bun, Electron, Go, C#, Rust,
Swift, and the native C/C++ archive publish against that same logger native
package, with the JVM Maven lane at `1.2.1-gf50756ebff0d`. Bun publishes
through connector source commit `6fdcc69`. Tauri and Electron publish through
connector source commit `3e8a6ae`. Mojo and Zig also publish current source
connector packages over that native generation through connector source commit
`8264bba`. The published logger package surface now bundles `macOS`, `Linux`,
and `Windows` logger natives across the binary and source release lanes, with
SwiftPM currently scoped to macOS ARM64.

Repository rules:

- keep this as a plain artifact repository, not a source-build repository
- store released drops under product lane, then language lane
- keep `manifest.json`, `SHA256SUMS`, and one short release `README.md` with
  each drop
- keep public artifacts aligned with
  [`docs/public-artifact-contract.md`](docs/public-artifact-contract.md)
- keep connector onboarding aligned with
  [`docs/connector-user-experience-levels.md`](docs/connector-user-experience-levels.md)

Current published lanes:

- `npm: coakka-v2-connector-node@1.3.9`
- `npm: coakka-v2-connector-bun@1.3.9`
- `npm: coakka-v2-connector-electron@1.3.9`
- `PyPI: coakka-v2-connector==1.3.4`
- `PyPI: coakka-logger==1.2.2`
- `SwiftPM: https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `1.3.2`
- `SwiftPM: https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1`
- `NuGet: CoAkka.Runtime@1.3.3`
- `NuGet: CoAkka.Logger@1.2.2`
- `npm: coakka-logger-node@1.2.6`
- `npm: coakka-logger-bun@1.2.6`
- `npm: coakka-logger-electron@1.2.6`
- `logger/jvm`
- `logger/python`
- `logger/node`
- `logger/bun`
- `logger/electron`
- `logger/go`
- `logger/csharp`
- `logger/rust`
- `logger/tauri`
- `logger/mojo`
- `logger/zig`
- `logger/native`
- static Maven repo under `maven/`
  - `coakka.logger:coakka-jvm-native-logger`
  - `coakka.v2:coakka-jvm-native-runtime-v2`
  - `coakka.spring:coakka-spring-boot-starter`
  - `coakka.quarkus:coakka-quarkus-extension`
- runtime v2 public `C` ABI files under `include/` and `native/`
- runtime v2 native C ABI archive under `runtime/native/releases/`
- runtime connector packages under `runtime/{jvm,python,node,bun,electron,go,csharp,rust}/releases/`
- runtime source connector packages under `runtime/{mojo,zig,tauri}/releases/`
- SwiftPM runtime/logger packages are published through public GitHub tags and
  tracked in `package-manager/swiftpm/current.json`
- coakka-client native CLI archives under `coakka-tools/coakka-client/releases/`
- coakka-client Docker Linux bundles under
  `coakka-tools/coakka-client/docker-demo/releases/`
- coakka-runtime-inspect native UI archives under
  `coakka-tools/coakka-runtime-inspect/releases/`
- Docker Hub sample image tags are recorded in the release notes that publish
  them; they are not part of the current release ledger yet.

## coakka-client Public Artifacts

Current coakka-client source snapshot: `caff6d6d`

Package contents:

- `coakka-tools/coakka-client/releases/1.3.2+caff6d6d/coakka-client-v2-1.3.2-linux-x86_64.tar.gz`
- `coakka-tools/coakka-client/releases/1.3.2+caff6d6d/coakka-client-v2-1.3.2-linux-aarch64.tar.gz`
- `coakka-tools/coakka-client/releases/1.3.2+caff6d6d/coakka-client-v2-1.3.2-macos-aarch64.tar.gz`
- `coakka-tools/coakka-client/releases/1.3.2+caff6d6d/coakka-client-v2-1.3.2-windows-x86_64.tar.gz`
- `coakka-tools/coakka-client/releases/1.3.2+caff6d6d/coakka-client-v2-1.3.2-windows-aarch64.tar.gz`
- `coakka-tools/coakka-client/docker-demo/releases/1.3.2+caff6d6d/coakka-client-docker-demo-v2-1.3.2-linux-x86_64.tar.gz`
- `coakka-tools/coakka-client/docker-demo/releases/1.3.2+caff6d6d/coakka-client-docker-demo-v2-1.3.2-linux-aarch64.tar.gz`

`coakka-client` is the CLI runtime client for diagnostics, request/reply
`call`/`ask`, and bounded shell script mode over the current public TCP frame
profile. It is not the dashboard, inspect surface, topology authority, or a
business schema registry.

The Docker Linux bundle starts two native runtime service processes and drives
them with the packaged `coakka-client`. Docker Hub sample images are a
convenience path recorded in the release notes that publish them; the canonical
binary archives and checksums remain in the release artifacts above.

Release note:
[`docs/releases/2026-07-25-runtime-tooling-1.3.2-caff6d6d.md`](docs/releases/2026-07-25-runtime-tooling-1.3.2-caff6d6d.md)

## coakka-runtime-inspect Public Artifacts

Current coakka-runtime-inspect source snapshot: `caff6d6d`

Package contents:

- `coakka-tools/coakka-runtime-inspect/releases/1.3.2+caff6d6d/coakka-runtime-inspect-v2-1.3.2-linux-aarch64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/1.3.2+caff6d6d/coakka-runtime-inspect-v2-1.3.2-linux-x86_64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/1.3.2+caff6d6d/coakka-runtime-inspect-v2-1.3.2-macos-aarch64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/1.3.2+caff6d6d/coakka-runtime-inspect-v2-1.3.2-windows-x86_64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/1.3.2+caff6d6d/coakka-runtime-inspect-v2-1.3.2-windows-aarch64.tar.gz`

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`, not an admin
dashboard, schema registry, service discovery server, mTLS control plane, or
business schema owner.

The native archives include:

- `bin/coakka-runtime-inspect`
- `bin/coakka-runtime-inspect.exe` on Windows
- `lib/libcoakka_runtime_v2.so` on Linux
- `lib/libcoakka_runtime_v2.dylib`
- `bin/libcoakka_runtime_v2.dll` on Windows

Current limits:

- `/api/snapshot` is labelled `local-linked-runtime`; remote read/observe is a
  future runtime surface.
- `serve --connect host:port` configures route try against a runtime request
  path.
- Remote read/observe remains a future runtime surface across all platforms.

Release notes:
[`docs/releases/2026-07-25-runtime-tooling-1.3.2-caff6d6d.md`](docs/releases/2026-07-25-runtime-tooling-1.3.2-caff6d6d.md)

## Runtime v2 Public Artifacts

Core source snapshot: `caff6d6d`

Package contents:

- `include/coakka/v2/runtime.h`
- `include/coakka/v2/control.h`
- `include/coakka/v2/client.h`
- `include/coakka/v2/transport.h`
- `include/coakka/v2/utils.h`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/windows-aarch64/libcoakka_runtime_v2.dll`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`
- `SHA256SUMS`
- `runtime/native/releases/1.3.2+caff6d6d/coakka-runtime-native-v2-1.3.2.tar.gz`

The staged native libraries include the runtime C ABI and scanner-clean
platform libraries for the public artifact surface. The host-facing routing
contract remains the same across profiles: targets, route generations,
request/reply, deadletters, and diagnostics stay in the public runtime
contract.

Runtime JVM now uses the `1.3.2-gcaff6d6d-6d5ea58` connector refresh over
native package `1.3.2+caff6d6d`. Spring Boot and Quarkus adapters now use
`1.3.2-gcaff6d6d-6d5ea58` over that JVM runtime. Embedded-native coverage in
the published JVM jar includes `macos-aarch64`, `linux-aarch64`,
`linux-x86_64`, `windows-aarch64`, and `windows-x86_64`. Python, Go, C#,
Rust, Mojo, Zig, and Tauri publish against `1.3.2+caff6d6d` through release
directory `1.3.2+caff6d6d-6d5ea58`. Node.js, Bun, and Electron publish as npm
package version `1.3.9` through release directory `1.3.2+caff6d6d-b46f705`.
Each release directory has its own manifest and checksums.

Native refresh note:
[`docs/releases/2026-07-25-runtime-native-1.3.2-caff6d6d.md`](docs/releases/2026-07-25-runtime-native-1.3.2-caff6d6d.md)

Language connector release notes:
[`docs/releases/2026-07-25-runtime-language-packages-1.3.2-caff6d6d-6d5ea58.md`](docs/releases/2026-07-25-runtime-language-packages-1.3.2-caff6d6d-6d5ea58.md),
[`docs/releases/2026-07-25-npm-runtime-native-bridge-b46f705.md`](docs/releases/2026-07-25-npm-runtime-native-bridge-b46f705.md),
[`docs/releases/2026-07-18-runtime-jvm-1.3.1-0a0aa76.md`](docs/releases/2026-07-18-runtime-jvm-1.3.1-0a0aa76.md),
[`docs/releases/2026-07-18-runtime-jvm-adapters-1.3.1-0a0aa76.md`](docs/releases/2026-07-18-runtime-jvm-adapters-1.3.1-0a0aa76.md),
[`docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md`](docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md),
[`docs/releases/2026-07-23-runtime-bun-tauri-1.3.1-247df1b.md`](docs/releases/2026-07-23-runtime-bun-tauri-1.3.1-247df1b.md),
[`docs/releases/2026-07-23-runtime-electron-1.3.1-4e0cab0.md`](docs/releases/2026-07-23-runtime-electron-1.3.1-4e0cab0.md)

Historical runtime connector release notes:
[`docs/releases/2026-05-23-runtime-jvm-adapters-c124a9e-1c2694b.md`](docs/releases/2026-05-23-runtime-jvm-adapters-c124a9e-1c2694b.md),
[`docs/releases/2026-05-23-runtime-jvm-c124a9e-2bab9ee.md`](docs/releases/2026-05-23-runtime-jvm-c124a9e-2bab9ee.md),
[`docs/releases/2026-05-21-runtime-connector-5ab812f.md`](docs/releases/2026-05-21-runtime-connector-5ab812f.md)
and
[`docs/releases/2026-05-17-runtime-94a5729.md`](docs/releases/2026-05-17-runtime-94a5729.md)

Source connector release note:
[`docs/releases/2026-07-25-runtime-language-packages-1.3.2-caff6d6d-6d5ea58.md`](docs/releases/2026-07-25-runtime-language-packages-1.3.2-caff6d6d-6d5ea58.md)

## Runtime Compatibility Matrix

These artifacts are the current public runtime set:

| Surface | Artifact | Version | Native package |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.2+caff6d6d/coakka-runtime-native-v2-1.3.2.tar.gz` | `1.3.2+caff6d6d` | `1.3.2+caff6d6d` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.3.2-gcaff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Python runtime | `coakka-v2-connector` PyPI package | PyPI `1.3.4` | `1.3.2+caff6d6d` |
| Node.js runtime | `coakka-v2-connector-node` package | npm `1.3.9` from connector `b46f705` | `1.3.2+caff6d6d` |
| Bun runtime | `coakka-v2-connector-bun` package | npm `1.3.9` from connector `b46f705` | `1.3.2+caff6d6d` |
| Electron runtime | `coakka-v2-connector-electron` package | npm `1.3.9` from connector `b46f705` | via `coakka-v2-connector-node@1.3.9` |
| Go runtime | `coakka-v2-connector-go` source package | `1.3.2` from `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| C# runtime | `CoAkka.Runtime` NuGet package | `1.3.3` | `1.3.2+caff6d6d` |
| Rust runtime | `coakka-runtime-rs` package | `1.3.2-spike` from `1.3.2+caff6d6d-6d5ea58` | `1.3.2+caff6d6d` |
| Mojo runtime | `runtime/mojo/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-mojo-1.3.2-source.tar.gz` | `1.3.2-source` | `1.3.2+caff6d6d` |
| Zig runtime | `runtime/zig/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-zig-1.3.2-source.tar.gz` | `1.3.2-source` | `1.3.2+caff6d6d` |
| Tauri runtime | `runtime/tauri/releases/1.3.2+caff6d6d-6d5ea58/coakka-runtime-tauri-intents-1.3.2-source.tar.gz` | `1.3.2-source` | `1.3.2+caff6d6d` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.3.2-gcaff6d6d-6d5ea58` | via JVM runtime `1.3.2-gcaff6d6d-6d5ea58` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.3.2-gcaff6d6d-6d5ea58` | via JVM runtime `1.3.2-gcaff6d6d-6d5ea58` |

Do not mix runtime language packages from another native package generation
unless a later release note explicitly declares that combination compatible.

## Connector User Experience

The public connector onboarding contract is
[`docs/connector-user-experience-levels.md`](docs/connector-user-experience-levels.md).
It defines the intended progression for upcoming connector releases:
local handler/ask first, explicit route snapshots second, custom
envelope/transport policy last.

Release docs and samples must only present a helper as released after the
published artifact contains it. The current npm runtime connector refresh in
`b46f705` uses native generation `1.3.2+caff6d6d` and keeps the JavaScript
transport serialization details behind the package boundary. The broader runtime
connector refresh in `6d5ea58` remains the source for the non-npm language lanes.

Validation gates run before publishing:

- Linux `aarch64` build and runtime tests
- Linux `x86_64` build and runtime tests
- macOS `aarch64` build and runtime tests
- Windows `aarch64` build and runtime tests
- Windows `x86_64` build and runtime tests
- native dependency scan
- public runtime header render test
- public artifact surface scan

Local repository verification:

```bash
COAKKA_PUBLIC_SURFACE_SCANNER=/path/to/scan_public_artifact_surface.sh \
  scripts/verify-public-surface.sh
```

The script verifies the root runtime checksums, every logger/runtime/CLI/Docker
bundle release checksum file, Maven checksum sidecars, runtime JVM release and
Maven jars against the current public native runtime package, Spring Boot and
Quarkus adapter dependency parity against the current runtime JVM Maven release,
`artifacts/public-artifacts.tsv`, and the optional content scanner when the
scanner path is provided. The scanner ignores the repo-local `.tmp` work area
so root scans do not recursively inspect their own extracted archives.

`artifacts/public-artifacts.tsv` is the consumer-facing package manifest. Each
row is status, label, relative path, and SHA256. The public surface gate rejects
rows outside the current public release surface, duplicate labels, duplicate
paths, missing files, and checksum mismatches.

Current integrity metadata is checksum-based: release manifests, `SHA256SUMS`,
Maven checksum sidecars, and `artifacts/public-artifacts.tsv`. Signature,
SBOM, and attestation files are not part of this release surface yet; add them
as explicit release artifacts when the signing flow is introduced.

Before adding a runtime JVM, Python, Node.js, Bun, Electron, Go, C#, Rust, Mojo, or Zig package release,
run the intake gate on the submitted artifact:

```bash
scripts/verify-runtime-intake-artifact.py \
  --lane python \
  --artifact /path/to/coakka_v2_connector-1.3.2-py3-none-any.whl \
  --expected-native-version 1.3.2+caff6d6d
```

Use the matching `--lane` value for the submitted package. The gate verifies
the bundled native package version, rejects workspace/sample/test paths, rejects
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
- `logger/bun/releases/`
- `logger/electron/releases/`
- `logger/go/releases/`
- `logger/csharp/releases/`
- `logger/rust/releases/`
- `logger/tauri/releases/`
- `logger/mojo/releases/`
- `logger/zig/releases/`
- `logger/native/releases/`

The logger lanes keep their own release manifests and checksums.

## Maven Artifacts

The static Maven repository lives under `maven/`. It exposes the logger JVM
artifact, runtime JVM artifact, Spring Boot starter, and Quarkus extension.
