# CoAkka Publish

<p align="center">
  <img src="docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

[![public-surface](https://github.com/phuong-tran/coakka-publish/actions/workflows/public-surface.yml/badge.svg)](https://github.com/phuong-tran/coakka-publish/actions/workflows/public-surface.yml)

**CoAkka is a polyglot, multi-language, multi-platform runtime ecosystem.**
One native core and a stable public C ABI support native C/C++, JVM and
framework adapters, JavaScript runtimes, Python, Go, C#, Rust, Swift, Zig,
Mojo, and related host integrations according to each release's compatibility
row.

Kubernetes is a first-class deployment lane, not a prerequisite. CoAkka keeps
the same target, request/reply, bounded-admission, and deadletter contract on
standalone Linux, macOS and Windows hosts, containers, VMs, bare metal, and
architecture-matched edge deployments. Start with the
[Ecosystem Overview](docs/ecosystem-overview.md) and use the
[Compatibility Matrix](docs/compatibility-matrix.md) for exact package, OS,
CPU, and release-channel evidence.

## Architecture Boundary

HTTP remains the external API edge; selected application-owned work crosses a
bounded CoAkka request/reply boundary instead of becoming another internal HTTP
service. Read
[Keep HTTP At The Edge](docs/http-edge-runtime-boundary.md) and
[Should I Choose Bun Over Node.js To Make CoAkka Faster?](docs/qna.md#should-i-choose-bun-over-nodejs-to-make-coakka-faster).
For browser streaming, read
[WebSocket Integration With CoAkka](docs/runtime-websocket-integration.md).

Contribution guide: [CONTRIBUTING.md](CONTRIBUTING.md)
Support: [SUPPORT.md](SUPPORT.md)
Contact: `gabrielgun1983@gmail.com`
Changelog: [CHANGELOG.md](CHANGELOG.md)
Troubleshooting: [docs/troubleshooting.md](docs/troubleshooting.md)
New to CoAkka: [docs/new-to-coakka.md](docs/new-to-coakka.md)
Startup development path: [docs/startup-development-path.md](docs/startup-development-path.md)
Runtime field guide: [docs/runtime-field-guide.md](docs/runtime-field-guide.md)
How it works: [docs/how-it-works.md](docs/how-it-works.md)
HTTP edge and runtime boundary:
[docs/http-edge-runtime-boundary.md](docs/http-edge-runtime-boundary.md)
WebSocket integration:
[docs/runtime-websocket-integration.md](docs/runtime-websocket-integration.md)
Questions and answers: [docs/qna.md](docs/qna.md)
Runtime client: [docs/coakka-runtime-client.md](docs/coakka-runtime-client.md)
Runtime inspect: [docs/coakka-runtime-inspect.md](docs/coakka-runtime-inspect.md)
Compatibility matrix: [docs/compatibility-matrix.md](docs/compatibility-matrix.md)
Package and platform evidence: [docs/runtime-package-platform-evidence.md](docs/runtime-package-platform-evidence.md)
Runtime addons: [docs/runtime-addons.md](docs/runtime-addons.md)
Package manager availability: [docs/package-manager-roadmap.md](docs/package-manager-roadmap.md)
PyPI package lane: [package-manager/pypi/README.md](package-manager/pypi/README.md)
Spring Boot: [docs/coakka-spring-boot.md](docs/coakka-spring-boot.md)
Quarkus: [docs/coakka-quarkus.md](docs/coakka-quarkus.md)
Runtime glossary: [docs/runtime-glossary.md](docs/runtime-glossary.md)
Envelope and deadletter map: [docs/envelope-deadletter-map.md](docs/envelope-deadletter-map.md)
Runtime integration guide: [docs/runtime-integration-guide.md](docs/runtime-integration-guide.md)
Runtime network modes: [docs/runtime-network-modes.md](docs/runtime-network-modes.md)
AI-assisted integration: [docs/ai-assisted-integration.md](docs/ai-assisted-integration.md)
Incremental adoption: [docs/incremental-adoption.md](docs/incremental-adoption.md)
Production readiness: [docs/production-readiness.md](docs/production-readiness.md)

This repository is a versioned artifact warehouse and catalog for independent
CoAkka components. It has no aggregate product version and does not publish
GitHub Releases. Select an exact lane path or package-manager coordinate, then
verify its manifest and checksum evidence.

## Featured Binary Lanes

These binaries are committed directly to this repository and become public
when the reviewed change is merged to `main`. They are not GitHub Release
attachments.

| Product | Binary lane | Source and consumer |
| --- | --- | --- |
| [Native artifact publishers](runtime-addons/README.md) | Eleven addons at `1.1.0+d1032f6d`; SFTP at `1.2.0+88b9a047`. | [`coakka-samples/runtime-addons/`](https://github.com/phuong-tran/coakka-samples/tree/main/runtime-addons) |
| [Raspberry Pi camera livestream](samples/runtime/native/rpi-camera/README.md) | `samples/runtime/native/rpi-camera/releases/1.1.0/` | [`coakka-samples/runtime-streaming-demo/rpi-camera/`](https://github.com/phuong-tran/coakka-samples/tree/main/runtime-streaming-demo/rpi-camera) |

## Runtime Addons

[`runtime-addons/`](runtime-addons/README.md) is the independent artifact family
for optional native capabilities that compose with CoAkka Runtime without
entering the default runtime package. Addons have their own versions,
compatibility manifests, dependency closure, checksums, and matching-host
evidence.

The artifact-source family acquires immutable external artifacts, verifies
exact size and SHA-256, then distributes them through File Lane. Eleven addons
are public at native `1.1.0+d1032f6d`; SFTP is public at replacement native
`1.2.0+88b9a047`. Read the common
[Runtime Addons](docs/runtime-addons.md) guide before generating integration
code. This addon family is native-first. Addon-specific language connectors
may be considered when demonstrated demand justifies their platform,
packaging, testing, and maintenance cost; none is part of the current release
contract or a committed roadmap.

## Runtime Transport

Runtime transport configuration is available through the full host-language
connectors. Use the canonical guides for the supported modes, effective
capabilities, lifecycle rules, and connector examples:

- [TLS and mTLS](docs/tls-and-mtls.md)
- [Connection strategies](docs/connection-strategies.md)
- [Runtime network modes](docs/runtime-network-modes.md)
- [Runtime file transfer](docs/runtime-file-transfer.md)
- [Runtime streaming](docs/runtime-streaming.md)
- [WebSocket integration](docs/runtime-websocket-integration.md)
- [AI-assisted integration](docs/ai-assisted-integration.md)
- [Envelope and deadletter map](docs/envelope-deadletter-map.md)
- [Message and routing model](docs/runtime-message-and-routing-model.md)

Runtime `2.4.0` contains the bounded file lane for large point-to-point files. The
sender reads from a file descriptor and the receiver writes to an
application-approved storage path. Direct transport can use the operating
system file-transfer primitive; TLS and mTLS use encrypted streaming. This is
not an end-to-end zero-copy claim because receiver-side persistence still
writes storage. The current release contains native artifacts for Linux
ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64, plus the matching
connector, framework, tool, sample, manifest, and checksum surfaces.

Runtime `2.4.0` also contains the official Stream Lane contract and reports the
`stream_lane` runtime feature. It carries bounded frames, backpressure,
lifecycle, security, and neutral publisher/subscriber pressure signals without
changing stream wire version 1. See
[Runtime streaming](docs/runtime-streaming.md) for the application boundary and
artifact-availability contract. A WebSocket-facing host composes with that
lane; it is not a Runtime WebSocket transport. See
[WebSocket integration](docs/runtime-websocket-integration.md).

## Runtime Test

The root-level [`runtime-test/`](runtime-test/README.md) is the auditable C11
consumer harness for the published native runtime. It uses only the stable
public C ABI and covers request/reply invariants, bounded admission, all four
connection strategies, structured rejection, static analysis, and
consumer-side ASan/UBSan controls. It also contains multi-producer race,
submit-versus-stop, independent lifecycle, and atomic route-snapshot hot-reload
evidence with a separate source-level TSan gate. Prebuilt platform runners
remain under
[`runtime/evidence/native/releases/`](runtime/evidence/native/releases/) for
systems without a local C toolchain.

This repository is the public package and artifact surface for CoAkka. The
runtime implementation is not included; public consumer headers, samples, and
the auditable `runtime-test/` harness remain inspectable.
If you are new to CoAkka, start with
[New To CoAkka](docs/new-to-coakka.md), then use
[`coakka-samples`](https://github.com/phuong-tran/coakka-samples) to run
examples. After one sample, read
[Runtime Field Guide](docs/runtime-field-guide.md) to connect the sample to
Kubernetes Service DNS, topology, queues, overload, Nginx, mTLS placement, and
route generation.

CoAkka Runtime is a native-backed capability runtime for application-owned
work across processes and languages. It helps an app route work by target
name, handle request/reply, deadletters, bounded queues, and diagnostics
without turning every internal boundary into another hand-written HTTP
endpoint.

Application-owned work means capability code governed by the same product or
application boundary, even when it runs in another process, language,
container, or host.

CoAkka Logger is a separate bounded logging surface in the same ecosystem.

## Start Here

| Question | Short answer |
| --- | --- |
| Problem | Internal application work often becomes an HTTP-shaped handoff only to give capability code an address, spreading one contract across URLs, clients, retries, timeout mapping, status mapping, and logs. |
| What CoAkka is | A runtime boundary for application capabilities: callers submit an identified payload to a stable target, route snapshots decide ownership, and replies/deadletters carry runtime diagnostics. |
| What this repo is | The public artifact surface: packages, native archives, optional runtime-addon releases, manifests, checksums, compatibility matrix, and release notes. |
| What it is not | Not the runnable sample repo and not a source-build repository. Use `coakka-samples` to run examples. |
| How to adopt | Pick one painful internal boundary, keep legacy HTTP/gRPC edges intact, and migrate gradually where runtime targets make ownership clearer. |

Public repository map:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |
| `coakka-samples` | Runnable examples that show how an app uses the runtime and logger packages. | https://github.com/phuong-tran/coakka-samples |
| `coakka-runtime-go` | Public Go module for CoAkka Runtime. | https://github.com/phuong-tran/coakka-runtime-go |
| `coakka-logger-go` | Public Go module for CoAkka Logger. | https://github.com/phuong-tran/coakka-logger-go |
| `coakka-runtime-swift` | Public SwiftPM package with five native payloads and macOS ARM64 Swift execution. | https://github.com/phuong-tran/coakka-runtime-swift |
| `coakka-logger-swift` | Public SwiftPM package for CoAkka Logger on macOS ARM64. | https://github.com/phuong-tran/coakka-logger-swift |

Current package-manager entrypoints:

Package versions are independent across the ecosystem. A NuGet package, npm
package, PyPI package, Go module, and SwiftPM tag do not need to share the same
version number.

| Channel | Runtime package | Logger package |
| --- | --- | --- |
| Maven Central | [`coakka.runtime` 2.4.1](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/runtime/2.4.1) | [`coakka.logger` 1.2.2](https://central.sonatype.com/artifact/io.github.phuong-tran.coakka/logger/1.2.2) |
| NuGet | [`CoAkka.Runtime` 2.4.1](https://www.nuget.org/packages/CoAkka.Runtime/2.4.1) | [`CoAkka.Logger` 1.2.3](https://www.nuget.org/packages/CoAkka.Logger/1.2.3) |
| npm | [`coakka-v2-connector-node` 2.4.0](https://www.npmjs.com/package/coakka-v2-connector-node/v/2.4.0) | [`coakka-logger-node` 1.2.6](https://www.npmjs.com/package/coakka-logger-node/v/1.2.6) |
| PyPI | [`coakka-v2-connector` 2.4.0](https://pypi.org/project/coakka-v2-connector/2.4.0/) | [`coakka-logger` 1.2.2](https://pypi.org/project/coakka-logger/1.2.2/) |
| Go modules | [`coakka-runtime-go` v1.7.1](https://github.com/phuong-tran/coakka-runtime-go/tree/v1.7.1) | [`coakka-logger-go` v1.2.6](https://pkg.go.dev/github.com/phuong-tran/coakka-logger-go@v1.2.6) |
| SwiftPM | [`coakka-runtime-swift` v2.4.1](https://github.com/phuong-tran/coakka-runtime-swift/tree/v2.4.1) | [`coakka-logger-swift` v1.2.2](https://github.com/phuong-tran/coakka-logger-swift/releases/tag/v1.2.2) |

Public docs:
[New To CoAkka](docs/new-to-coakka.md),
[Build The Product Before The Infrastructure](docs/startup-development-path.md),
[Current Packages](docs/current-packages.md),
[Runtime Field Guide](docs/runtime-field-guide.md),
[How It Works](docs/how-it-works.md),
[Questions And Answers](docs/qna.md),
[The CoAkka Story](docs/coakka-story.md),
[AI Reviewer Onboarding](docs/ai-reviewer-onboarding.md),
[Architecture Review Guide](docs/architecture-review-guide.md),
[CoAkka Spring Boot](docs/coakka-spring-boot.md),
[CoAkka Quarkus](docs/coakka-quarkus.md),
[CoAkka Runtime Client](docs/coakka-runtime-client.md),
[CoAkka Runtime Inspect](docs/coakka-runtime-inspect.md),
[Runtime Glossary](docs/runtime-glossary.md),
[Runtime Message And Routing Model](docs/runtime-message-and-routing-model.md),
[Envelope And Deadletter Map](docs/envelope-deadletter-map.md),
[Runtime Integration Guide](docs/runtime-integration-guide.md),
[C# Transport Configuration](docs/csharp-transport-configuration.md),
[Native C++ Transport Configuration](docs/native-cpp-transport-configuration.md),
[Incremental Adoption](docs/incremental-adoption.md),
[Cluster Routing](docs/runtime-cluster-routing.md),
[Containerized Runtime](docs/containerized-runtime.md),
[Production Readiness](docs/production-readiness.md),
[Production Evidence](docs/production-evidence.md),
[Runtime Package And Platform Evidence](docs/runtime-package-platform-evidence.md),
[Runtime Addons](docs/runtime-addons.md),
and
[Repository Boundaries](docs/repository-boundaries.md).

Use [`artifacts/public-artifacts.tsv`](artifacts/public-artifacts.tsv) and
[`docs/compatibility-matrix.md`](docs/compatibility-matrix.md) as the current
public artifact index. Versioned files are downloaded from their immutable lane
paths in this repository; npm, NuGet, and PyPI packages are downloaded from
their registries.

Current runtime release note:
[2026-08-11 runtime 2.4.0](docs/releases/2026-08-11-runtime-2.4.0-c2f53117.md).

Current package-manager release notes:
[2026-08-17 NuGet .NET app-host baseline](docs/releases/2026-08-17-dotnet-nuget-app-host-baseline.md),
[2026-08-12 npm JavaScript runtime 2.4.0](docs/releases/2026-08-12-npm-runtime-2.4.0.md),
[2026-08-12 NuGet C# runtime 2.4.0](docs/releases/2026-08-12-nuget-csharp-runtime-2.4.0.md),
[2026-08-12 PyPI Python runtime 2.4.0](docs/releases/2026-08-12-pypi-python-runtime-2.4.0.md),
[2026-08-12 Go runtime v1.7.0](docs/releases/2026-08-12-go-runtime-v1.7.0.md),
[2026-08-17 Go runtime v1.7.1](docs/releases/2026-08-17-go-runtime-v1.7.1.md),
[2026-08-17 SwiftPM macOS 13 payload correction](docs/releases/2026-08-17-swiftpm-macos13-payload-correction.md),
[2026-08-12 SwiftPM runtime 2.4.0](docs/releases/2026-08-12-swiftpm-runtime-2.4.0.md),
[2026-08-09 Go and Swift documentation-integrity patches](docs/releases/2026-08-09-go-swift-documentation-integrity.md),
and earlier package-manager receipts under [docs/releases/](docs/releases/).

Current runtime tools release note:
[2026-08-12 runtime tools 2.4.0](docs/releases/2026-08-12-runtime-tools-2.4.0-c2f53117.md).
Last documented coakka-runtime-inspect Docker Hub release note:
[2026-07-25 runtime tooling stop backpressure hotfix 0da8c2d9](docs/releases/2026-07-25-runtime-tooling-stop-backpressure-hotfix-0da8c2d9.md).

Historical GitHub Release boundary note:
[2026-07-18 public release boundary 1.3.1](docs/releases/2026-07-18-public-release-boundary-1.3.1.md).

Current language connector release notes:
[2026-08-11 runtime 2.4.0](docs/releases/2026-08-11-runtime-2.4.0-c2f53117.md),
[2026-08-17 NuGet .NET app-host baseline](docs/releases/2026-08-17-dotnet-nuget-app-host-baseline.md),
[2026-08-12 npm JavaScript runtime 2.4.0](docs/releases/2026-08-12-npm-runtime-2.4.0.md),
[2026-08-09 Go and Swift documentation-integrity patches](docs/releases/2026-08-09-go-swift-documentation-integrity.md),
[2026-08-12 C# runtime NuGet 2.4.0](docs/releases/2026-08-12-nuget-csharp-runtime-2.4.0.md),
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
[2026-08-09 runtime 2.1.0](docs/releases/2026-08-09-runtime-2.1.0-60ddf70d.md).

## License And Trademark

This repository distributes CoAkka artifacts under the
[CoAkka Public Artifact License 1.1](LICENSE.md), unless a specific artifact
includes explicitly different terms. This public-use artifact license is not
an OSI-approved open source license.

The license permits royalty-free use of every included capability in
development, CI, production, internal business systems, commercial
applications, and SaaS products that use CoAkka as an internal component. It
also permits distributing unmodified artifacts as part of an application when
the required notices remain available.

A separate agreement is required only when the offering itself is a Managed
CoAkka Service or when CoAkka provides the primary or substantial value of a
standalone runtime, appliance, cloud image, infrastructure product, or
marketplace offering. The restriction is based on what is offered, not on the
size or industry of the user.

The separate `coakka-samples` repository contains sample code and documentation
under its own license. That sample license does not change the terms for the
runtime binaries, connector packages, Maven artifacts, or other released
artifacts distributed here.

For Managed CoAkka Service rights or another separately licensed use, use the
contact path in [SUPPORT.md](SUPPORT.md).

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
Current public native runtime generation: `2.4.0+c2f53117`.
Current public JVM mirror generation: `2.4.0-gc2f53117-0afb5e9`; Maven Central
publishes the independently versioned JVM distribution as `runtime:2.4.1`.
Current public Spring Boot and Quarkus Maven Central adapter versions are
`spring-boot-starter:2.4.1` and `quarkus-extension:2.4.1`.
Current public source connector artifact generation:
`2.4.0+c2f53117-0afb5e9`.
Current public npm registry candidate generation: `2.4.0+c2f53117-0afb5e9`.
Current public Node.js, Bun, and Electron npm runtime packages:
`coakka-v2-connector-{node,bun,electron}@2.4.0`.
Current public Python PyPI runtime package:
`coakka-v2-connector==2.4.0`, over native generation `2.4.0+c2f53117`.
Current public C# runtime NuGet package: `CoAkka.Runtime@2.4.1`, over native
generation `2.4.0+c2f53117`.
Current public C# logger NuGet package: `CoAkka.Logger@1.2.3`, over native
generation `1.2.1+f50756ebff0d`.
Current public Mojo/Zig source connector generation:
`2.4.0+c2f53117-0afb5e9`.
Current public Tauri intent source connector generation:
`2.4.0+c2f53117-0afb5e9`.
Current public coakka-client generation: `2.4.0+c2f53117` on all five
supported native platforms.
Current public coakka-client Docker Linux bundle generation: `1.3.2+caff6d6d`.
Current public coakka-runtime-inspect native generation: `2.4.0+c2f53117` on
all five supported native platforms.
Docker Hub image tags are not part of the current release ledger yet; the
Repository-hosted demo bundles above are the captured public sample artifacts.

| Lane | Public status | Public location |
| --- | --- | --- |
| Logger JVM, Python, Node.js, Bun, Electron, Go, C#, Rust | public | `logger/{jvm,python,node,bun,electron,go,csharp,rust}/releases/` |
| Logger Swift | public SwiftPM | `https://github.com/phuong-tran/coakka-logger-swift`, tag `v1.2.2` |
| Logger Tauri, Mojo, and Zig source connector lanes | public source packages | `logger/{tauri,mojo,zig}/releases/` |
| Logger native C/C++ | public | `logger/native/releases/` |
| Runtime native C ABI | public | `runtime/native/releases/` |
| Runtime JVM, Python, Node.js, Bun, Electron, Go, C#, Rust, Swift | public artifacts | `runtime/{jvm,python,node,bun,electron,go,csharp,rust,swift}/releases/` |
| Runtime SwiftPM | public package-manager lane | `https://github.com/phuong-tran/coakka-runtime-swift`, current registry/tag status below |
| Runtime Mojo, Zig, and Tauri source connector lanes | public source packages | `runtime/{mojo,zig,tauri}/releases/` |
| Runtime Spring Boot and Quarkus adapters | public Maven Central packages | `io.github.phuong-tran.coakka:{spring-boot-starter,quarkus-extension}:2.4.1` |
| Root runtime headers and native libraries | public | `include/` and `native/` |
| coakka-client native CLI | public | `coakka-tools/coakka-client/releases/` |
| coakka-client Docker Linux bundle | public Linux bundles | `coakka-tools/coakka-client/docker-demo/releases/` |
| coakka-runtime-inspect native UI | public macOS ARM64, Linux x86_64/ARM64, and Windows x86_64/ARM64 | `coakka-tools/coakka-runtime-inspect/releases/` |
| Docker Hub sample images | not captured in the current release ledger | see per-release Docker Hub notes |

SwiftPM package-manager releases:

| Lane | Coordinate | Native generation |
| --- | --- | --- |
| Swift runtime connector | `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `2.4.1` | runtime native `2.4.0+c2f53117` |
| Swift logger connector | `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.2` | logger native `1.2.1+f50756ebff0d` |

The SwiftPM runtime tag `v2.4.1` contains all five native payloads and has matching-host
Swift execution on macOS ARM64. Linux carries strict C-bridge and payload
verification; Windows carries payload verification. The separately versioned
Swift logger package remains macOS ARM64. The release manifest is tracked under
`package-manager/swiftpm/current.json`.

NuGet package-manager releases:

| Lane | Coordinate | Native generation |
| --- | --- | --- |
| C# runtime connector | `CoAkka.Runtime`, exact `2.4.1` (`net8.0`; verified on .NET 8/9/10) | runtime native `2.4.0+c2f53117` |
| C# logger connector | `CoAkka.Logger`, exact `1.2.3` (`net8.0`; verified on .NET 8/9/10) | logger native `1.2.1+f50756ebff0d` |

The NuGet lane is binary package-manager distribution for .NET consumers.
Its release manifest is tracked under
`package-manager/nuget/current.json`.

The current root native runtime package is `2.4.0+c2f53117`. The artifact
mirror publishes JVM runtime, Spring Boot, and Quarkus as
`2.4.0-gc2f53117-0afb5e9`. Connector artifacts use release directory
`2.4.0+c2f53117-0afb5e9`. npm and PyPI remain registry-published at `2.4.0`
from connector source `0afb5e9`; NuGet Runtime `2.4.1` and Logger `1.2.3`
come from connector source `801a0a6`. All three runtime channels are
registry-verified.

Maven Central publishes the JVM Runtime, Spring Boot starter, and Quarkus
extension as independently versioned `2.4.1` packages. The static mirror
coordinates above remain immutable compatibility artifacts; new framework
adapter consumers should use Maven Central.

Package-manager registries are separate publication channels. npm Node.js,
Bun, and Electron plus PyPI runtime are current at `2.4.0`; NuGet runtime is
current at `2.4.1` and NuGet logger at `1.2.3`.
Go modules advance through `coakka-runtime-go@v1.7.1`; the module stays on
semantic major `v1` because its path has no `/v2` suffix. SwiftPM advances
through `coakka-runtime-swift@v2.4.1`.
Do not mix language or framework runtime packages from another native package
generation unless a release note explicitly declares that combination
compatible.

For development and integration work, macOS and Windows are both supported host
surfaces in the current train. The current published package surface bundles
macOS, Linux, and Windows runtime natives. Linux remains the normal deployment
path for release, soak, and operator-readiness claims.

Consumer-facing warehouse downloads live at immutable versioned paths and are
listed in `artifacts/public-artifacts.tsv`. Integrity metadata is checksum-based
for this surface: lane manifests, `SHA256SUMS`, Maven checksum sidecars, and the
root artifact manifest. Signature, SBOM, and attestation files are not part of
this artifact surface yet. The manifest also keeps explicit provenance comments
for public Mojo/Zig source-package lanes so the connector source commit is
visible at the package list boundary.

The current npm coordinates are listed per package because Node.js, Bun, and
Electron can advance independently; this release aligns all three at `2.4.0`.
The Python runtime package
`coakka-v2-connector==2.4.0` and logger package
`coakka-logger==1.2.2` are published on PyPI. Superseded PyPI versions
`coakka-v2-connector==1.3.2`, `coakka-v2-connector==1.3.3`, and
`coakka-logger==1.2.1` are yanked so ordinary installs resolve to the current
patch versions. The Go runtime package
`github.com/phuong-tran/coakka-runtime-go@v1.7.1` and logger package
`github.com/phuong-tran/coakka-logger-go@v1.2.6` are published as public Go
modules. SwiftPM packages for Swift runtime/logger are published as public
GitHub tag coordinates. NuGet packages for C# runtime/logger are published as
public package coordinates. Current package coordinates are summarized in
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

- `Maven Central: io.github.phuong-tran.coakka:runtime:2.4.1`
- `Maven Central: io.github.phuong-tran.coakka:logger:1.2.2`
- `npm: coakka-v2-connector-node@2.4.0`
- `npm: coakka-v2-connector-bun@2.4.0`
- `npm: coakka-v2-connector-electron@2.4.0`
- `PyPI: coakka-v2-connector==2.4.0`
- `PyPI: coakka-logger==1.2.2`
- `SwiftPM: https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `2.4.1`
- `SwiftPM: https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.2`
- `NuGet: CoAkka.Runtime@2.4.1`
- `NuGet: CoAkka.Logger@1.2.3`
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
- Maven Central coordinates
  `io.github.phuong-tran.coakka:runtime:2.4.1` and
  `io.github.phuong-tran.coakka:logger:1.2.2`, plus Java 17 framework adapters
  `io.github.phuong-tran.coakka:spring-boot-starter:2.4.1` and
  `io.github.phuong-tran.coakka:quarkus-extension:2.4.1`
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

Current coakka-client source snapshot: `c2f53117`

Package contents:

- `coakka-tools/coakka-client/releases/2.4.0+c2f53117/coakka-client-v2-2.4.0-linux-aarch64.tar.gz`
- `coakka-tools/coakka-client/releases/2.4.0+c2f53117/coakka-client-v2-2.4.0-linux-x86_64.tar.gz`
- `coakka-tools/coakka-client/releases/2.4.0+c2f53117/coakka-client-v2-2.4.0-macos-aarch64.tar.gz`
- `coakka-tools/coakka-client/releases/2.4.0+c2f53117/coakka-client-v2-2.4.0-windows-aarch64.tar.gz`
- `coakka-tools/coakka-client/releases/2.4.0+c2f53117/coakka-client-v2-2.4.0-windows-x86_64.tar.gz`
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
[`docs/releases/2026-08-12-runtime-tools-2.4.0-c2f53117.md`](docs/releases/2026-08-12-runtime-tools-2.4.0-c2f53117.md)

## coakka-runtime-inspect Public Artifacts

Current coakka-runtime-inspect source snapshot: `c2f53117`

Package contents:

- `coakka-tools/coakka-runtime-inspect/releases/2.4.0+c2f53117/coakka-runtime-inspect-v2-2.4.0-linux-aarch64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/2.4.0+c2f53117/coakka-runtime-inspect-v2-2.4.0-linux-x86_64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/2.4.0+c2f53117/coakka-runtime-inspect-v2-2.4.0-macos-aarch64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/2.4.0+c2f53117/coakka-runtime-inspect-v2-2.4.0-windows-aarch64.tar.gz`
- `coakka-tools/coakka-runtime-inspect/releases/2.4.0+c2f53117/coakka-runtime-inspect-v2-2.4.0-windows-x86_64.tar.gz`

`coakka-runtime-inspect` is the browser runtime explorer and route-try UI for
CoAkka Runtime. It is the visual sibling of `coakka-client`: a way to read and
try runtime facts, not the owner of fleet topology, business schema, or
production operations policy.

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

The macOS ARM64 and Linux ARM64/x86-64 packages completed matching-host command
and `serve` smoke. Windows ARM64/x86-64 passed cross-build, PE architecture,
dependency, archive, and checksum gates; this generation does not claim
matching-host Windows execution.

Release note:
[`docs/releases/2026-08-12-runtime-tools-2.4.0-c2f53117.md`](docs/releases/2026-08-12-runtime-tools-2.4.0-c2f53117.md)

## Runtime v2 Public Artifacts

Core source snapshot: `c2f53117f991f67f809a0bf46bac2ce26091eb78`

Package contents:

- `include/coakka/v2/runtime.h`
- `include/coakka/v2/file_lane.h`
- `include/coakka/v2/stream_lane.h`
- `include/coakka/v2/control.h`
- `include/coakka/v2/client.h`
- `include/coakka/v2/runtime_auth.h`
- `include/coakka/v2/runtime_distribution.h`
- `include/coakka/v2/runtime_transport_config.h`
- `include/coakka/v2/transport.h`
- `include/coakka/v2/utils.h`
- `native/linux-aarch64/libcoakka_runtime_v2.so`
- `native/linux-x86_64/libcoakka_runtime_v2.so`
- `native/macos-aarch64/libcoakka_runtime_v2.dylib`
- `native/windows-aarch64/libcoakka_runtime_v2.dll`
- `native/windows-x86_64/libcoakka_runtime_v2.dll`
- `SHA256SUMS`
- `runtime/native/releases/2.4.0+c2f53117/coakka-runtime-native-v2-2.4.0.tar.gz`

The staged native libraries include the runtime C ABI and scanner-clean
platform libraries for the public artifact surface. The host-facing routing
contract remains the same across profiles: targets, route generations,
request/reply, deadletters, and diagnostics stay in the public runtime
contract.

The static Runtime JVM mirror uses `2.4.0-gc2f53117-0afb5e9` over native
package `2.4.0+c2f53117`; its Spring Boot and Quarkus mirrors use the same
connector generation. Maven Central publishes Runtime, Spring Boot, and
Quarkus as `2.4.1` from their recorded release commits.
Embedded-native coverage is Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64. Every connector release directory has its own manifest and
checksums.

Native refresh note:
[`docs/releases/2026-08-11-runtime-2.4.0-c2f53117.md`](docs/releases/2026-08-11-runtime-2.4.0-c2f53117.md)

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
| Native C ABI | `runtime/native/releases/2.4.0+c2f53117/coakka-runtime-native-v2-2.4.0.tar.gz` | `2.4.0+c2f53117` | `2.4.0+c2f53117` |
| JVM runtime | `io.github.phuong-tran.coakka:runtime` | `2.4.1` | `2.4.0+c2f53117` |
| Python artifact mirror | `coakka_v2_connector` wheel | `2.4.0`, source `0afb5e9` | `2.4.0+c2f53117` |
| Node.js artifact mirror | `coakka-v2-connector-node` tarball | `2.4.0`, source `0afb5e9` | `2.4.0+c2f53117` |
| Bun artifact mirror | `coakka-v2-connector-bun` tarball | `2.4.0`, source `0afb5e9` | `2.4.0+c2f53117` |
| Electron artifact mirror | `coakka-v2-connector-electron` tarball | `2.4.0`, source `0afb5e9` | `2.4.0+c2f53117` |
| Go runtime artifact | `coakka-v2-connector-go` archive | `2.4.0` | `2.4.0+c2f53117` |
| C# artifact mirror | `CoAkka.Runtime` package file | `2.4.0` | `2.4.0+c2f53117` |
| Rust runtime artifact | `coakka-runtime-rs` package | `2.4.0` | `2.4.0+c2f53117` |
| Swift runtime artifact | `coakka-runtime-swift` package | `2.4.1` | `2.4.0+c2f53117` |
| Mojo runtime | `runtime/mojo/releases/2.4.0+c2f53117-0afb5e9/coakka-runtime-mojo-2.4.0-source.tar.gz` | `2.4.0-source` | `2.4.0+c2f53117` |
| Zig runtime | `runtime/zig/releases/2.4.0+c2f53117-0afb5e9/coakka-runtime-zig-2.4.0-source.tar.gz` | `2.4.0-source` | `2.4.0+c2f53117` |
| Tauri runtime | `runtime/tauri/releases/2.4.0+c2f53117-0afb5e9/coakka-runtime-tauri-intents-2.4.0-source.tar.gz` | `2.4.0-source` | `2.4.0+c2f53117` |
| Spring Boot adapter | `io.github.phuong-tran.coakka:spring-boot-starter` | `2.4.1` | via JVM runtime |
| Quarkus adapter | `io.github.phuong-tran.coakka:quarkus-extension` | `2.4.1` | via JVM runtime |

Do not mix runtime language packages from another native package generation
unless a later release note explicitly declares that combination compatible.

## Connector User Experience

The public connector onboarding contract is
[`docs/connector-user-experience-levels.md`](docs/connector-user-experience-levels.md).
It defines the intended progression for upcoming connector releases:
local handler/ask first, explicit route snapshots second, custom
envelope/transport policy last.

Release docs and samples present a helper only when the published artifact
contains it. The `2.4.0` artifact mirror uses connector source `0afb5e9`.
The registry-published npm and PyPI `2.4.0` packages use connector source
`0afb5e9`; NuGet Runtime `2.4.1` uses connector source `801a0a6`. All retain
native generation `2.4.0+c2f53117` and expose File Lane and Stream Lane.

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
  --artifact /path/to/coakka_v2_connector-1.4.6-py3-none-any.whl \
  --expected-native-version 1.4.1+9e02a51d
```

Use the matching `--lane` value for the submitted package. The gate verifies
the bundled native package version, rejects workspace/sample/test paths, rejects
stale native library names, and runs the recursive public surface scanner over
the full artifact contents.

For auxiliary artifacts that intentionally do not embed a native library, such
as a JVM sources jar, add `--allow-no-native`. Main runtime connector packages
must not use that flag.

## Runtime Addon Artifacts

Optional runtime capabilities release independently under
[`runtime-addons/`](runtime-addons/README.md). They do not become part of the
default runtime archive and must not make ordinary runtime consumers install
their implementation dependencies.

The released family is listed under [`runtime-addons/`](runtime-addons/README.md)
and in `artifacts/public-artifacts.tsv`. Every addon remains independent from
the Runtime archive and every connector package.

Before promotion, run:

```bash
scripts/verify-runtime-addon-release.py \
  --release-dir runtime-addons/<addon>/native/releases/<release>
scripts/test-runtime-addon-release.sh
scripts/verify-public-surface.sh
```

The intake gate checks runtime ABI/feature compatibility, the reviewed C ABI,
matching-host platform evidence, archive shape, checksums, exact static
dependency versions, and absence of bundled runtime or implementation sidecar
libraries. The full public-surface gate also inspects native linkage.

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
