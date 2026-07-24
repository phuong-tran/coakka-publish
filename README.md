# CoAkka Publish

[![public-surface](https://github.com/phuong-tran/coakka-publish/actions/workflows/public-surface.yml/badge.svg)](https://github.com/phuong-tran/coakka-publish/actions/workflows/public-surface.yml)

Contribution guide: [CONTRIBUTING.md](CONTRIBUTING.md)
Support: [SUPPORT.md](SUPPORT.md)
Changelog: [CHANGELOG.md](CHANGELOG.md)
Compatibility matrix: [docs/compatibility-matrix.md](docs/compatibility-matrix.md)
Release checklist: [docs/release-checklist.md](docs/release-checklist.md)
Package manager roadmap: [docs/package-manager-roadmap.md](docs/package-manager-roadmap.md)

This repository is the public binary-only publish surface for CoAkka artifacts.

Current GitHub Release page with attached public artifacts:
[CoAkka Public Artifacts 1.3.1](https://github.com/phuong-tran/coakka-publish/releases/tag/coakka-public-artifacts-v1.3.1).

Current native runtime refresh note:
[2026-07-18 runtime native 1.3.1 bda2ef5](docs/releases/2026-07-18-runtime-native-1.3.1-bda2ef5.md).

Current coakka-client release note:
[2026-07-18 coakka-client 1.3.1 2215b0f](docs/releases/2026-07-18-coakka-client-1.3.1-2215b0f.md).

Current coakka-runtime-inspect release notes:
[2026-07-19 coakka-runtime-inspect 1.3.1 d7ab7fa macOS ARM64](docs/releases/2026-07-19-coakka-runtime-inspect-1.3.1-d7ab7fa-macos-aarch64.md),
[2026-07-20 coakka-runtime-inspect 1.3.1 4ce41f19 Linux x86_64](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-x86_64.md),
[2026-07-20 coakka-runtime-inspect 1.3.1 4ce41f19 Linux ARM64](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-aarch64.md),
and
[2026-07-20 coakka-runtime-inspect 1.3.1 d7ab7fa Windows](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-d7ab7fa-windows.md).
Current coakka-runtime-inspect Docker Hub release note:
[2026-07-20 coakka-runtime-inspect Docker Hub image 1.3.1 4ce41f19](docs/releases/2026-07-20-coakka-runtime-inspect-dockerhub-1.3.1-4ce41f19.md).

Current public release boundary:
[2026-07-18 public release boundary 1.3.1](docs/releases/2026-07-18-public-release-boundary-1.3.1.md).

Current language connector release notes:
[2026-07-23 logger Tauri and Electron 1.2.1 refresh 3e8a6ae](docs/releases/2026-07-23-logger-tauri-electron-1.2.1-3e8a6ae.md),
[2026-07-23 logger Bun 1.2.1 refresh 6fdcc69](docs/releases/2026-07-23-logger-bun-1.2.1-6fdcc69.md),
[2026-07-06 logger 1.2.1 Windows parity 8264bba](docs/releases/2026-07-06-logger-1.2.1-8264bba.md),
[2026-07-05 logger Mojo and Zig source refresh 1107154](docs/releases/2026-07-05-logger-zig-mojo-source-1107154.md),
[2026-07-18 runtime JVM 1.3.1 refresh 0a0aa76](docs/releases/2026-07-18-runtime-jvm-1.3.1-0a0aa76.md),
[2026-07-18 runtime JVM adapters 1.3.1 refresh 0a0aa76](docs/releases/2026-07-18-runtime-jvm-adapters-1.3.1-0a0aa76.md),
[2026-07-18 runtime non-JVM 1.3.1 refresh 0a0aa76](docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md),
[2026-07-23 runtime Bun and Tauri 1.3.1 refresh 247df1b](docs/releases/2026-07-23-runtime-bun-tauri-1.3.1-247df1b.md),
and
[2026-07-23 runtime Electron 1.3.1 refresh 4e0cab0](docs/releases/2026-07-23-runtime-electron-1.3.1-4e0cab0.md).

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
[2026-07-18 runtime non-JVM 1.3.1 refresh 0a0aa76](docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md).

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
Current public native runtime generation: `1.3.1+bda2ef5`.
Current public JVM connector generation: `1.3.1+bda2ef5-0a0aa76`.
Current public Spring Boot and Quarkus adapter generation: `1.3.1-g0a0aa76`.
Current public Python, Node.js, Go, C#, and Rust connector generation:
`1.3.1+bda2ef5-0a0aa76`.
Current public Mojo/Zig source connector generation: `1.3.1+bda2ef5-0a0aa76`.
Current public Bun connector generation: `1.3.1+bda2ef5-247df1b`.
Current public Tauri intent source connector generation: `1.3.1+bda2ef5-247df1b`.
Current public Electron connector generation: `1.3.1+bda2ef5-4e0cab0`.
Current public coakka-client generation: `1.3.1+2215b0f`.
Current public coakka-client Docker Linux bundle generation: `1.3.1+2215b0f`.
Current public coakka-runtime-inspect native generations:
`1.3.1+4ce41f19` for Linux x86_64 and Linux ARM64; `1.3.1+d7ab7fa`
for macOS ARM64, Windows x86_64, and Windows ARM64.
Current public coakka-client Docker Hub demo image:
`docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote`.
Current public coakka-runtime-inspect Docker Hub sample image:
`docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote`.

| Lane | Public status | Public location |
| --- | --- | --- |
| Logger JVM, Python, Node.js, Bun, Electron, Go, C#, Rust | public | `logger/{jvm,python,node,bun,electron,go,csharp,rust}/releases/` |
| Logger Tauri, Mojo, and Zig source connector lanes | public source packages | `logger/{tauri,mojo,zig}/releases/` |
| Logger native C/C++ | public | `logger/native/releases/` |
| Runtime native C ABI | public | `runtime/native/releases/` |
| Runtime JVM, Python, Node.js, Bun, Electron, Go, C#, Rust | public | `runtime/{jvm,python,node,bun,electron,go,csharp,rust}/releases/` |
| Runtime Mojo, Zig, and Tauri source connector lanes | public source packages | `runtime/{mojo,zig,tauri}/releases/` |
| Runtime Spring Boot and Quarkus adapters | public | `maven/` |
| Root runtime headers and native libraries | public | `include/` and `native/` |
| coakka-client native CLI | public | `cli/releases/` |
| coakka-client Docker Linux bundle | public Linux bundles | `demo/coakka-client/releases/` |
| coakka-runtime-inspect native UI | public macOS ARM64, Linux x86_64/ARM64, and Windows x86_64/ARM64 | `runtime-inspect/native/releases/` |
| coakka-client Docker Hub demo image | public sample image | `docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote` |
| coakka-runtime-inspect Docker Hub sample image | public sample image | `docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote` |

The current root native runtime package is `1.3.1+bda2ef5`. The published JVM
runtime jar is refreshed over that native package as
`1.3.1-gbda2ef5-0a0aa76`. Published Spring Boot and Quarkus adapters are
rebuilt as `1.3.1-g0a0aa76` over that JVM runtime. Python, Node.js, Go, C#,
Rust, Mojo, and Zig also publish against the same `1.3.1+bda2ef5` native
package through connector source commit `0a0aa76`. Bun and the Tauri intent
source package publish against the same native package through connector source
commit `247df1b`. Electron publishes against the same native package through
connector source commit `4e0cab0`. Do not mix language or framework runtime packages from
another native package generation unless a release note explicitly declares
that combination compatible.

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
Go modules, crates.io, and apt/deb remain planned distribution surfaces. The
intended order and release gates are tracked in
[`docs/package-manager-roadmap.md`](docs/package-manager-roadmap.md).

The current public logger family keeps native package generation
`1.2.1+f50756ebff0d`. JVM, Python, Node.js, Bun, Electron, Go, C#, Rust, and
the native C/C++ archive publish against that same logger native package, with
the JVM Maven lane at `1.2.1-gf50756ebff0d`. Bun publishes through connector
source commit `6fdcc69`. Tauri and Electron publish through connector source
commit `3e8a6ae`. Mojo and Zig also publish current source connector packages
over that native generation through connector source commit `8264bba`. The
published logger package surface now bundles `macOS`, `Linux`, and `Windows`
logger natives across the binary and source release lanes.

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

- `npm: coakka-v2-connector-node@1.3.1`
- `npm: coakka-v2-connector-bun@1.3.1`
- `npm: coakka-v2-connector-electron@1.3.1`
- `npm: coakka-logger-node@1.2.1`
- `npm: coakka-logger-bun@1.2.1`
- `npm: coakka-logger-electron@1.2.1`
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
- coakka-client native CLI archives under `cli/releases/`
- coakka-client Docker Linux bundles under `demo/coakka-client/releases/`
- coakka-runtime-inspect native UI archives under `runtime-inspect/native/releases/`
- coakka-client Docker Hub demo image at
  `docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote`
- coakka-runtime-inspect Docker Hub sample image at
  `docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote`

## coakka-client Public Artifacts

Current coakka-client source snapshot: `2215b0f`

Package contents:

- `cli/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-linux-x86_64.tar.gz`
- `cli/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-linux-aarch64.tar.gz`
- `cli/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-macos-aarch64.tar.gz`
- `cli/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-windows-x86_64.tar.gz`
- `cli/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-windows-aarch64.tar.gz`
- `demo/coakka-client/releases/1.3.1+2215b0f/coakka-client-docker-demo-v2-1.3.1-linux-x86_64.tar.gz`
- `demo/coakka-client/releases/1.3.1+2215b0f/coakka-client-docker-demo-v2-1.3.1-linux-aarch64.tar.gz`

`coakka-client` is the CLI runtime client for diagnostics, request/reply
`call`/`ask`, and bounded shell script mode over the current public TCP frame
profile. It is not the dashboard, inspect surface, topology authority, or a
business schema registry.

The runtime-client Docker Hub demo image is:

```text
docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote
```

It starts two native runtime service processes and drives them with the
packaged `coakka-client`. This image is a sample convenience path; the
canonical binary archives and checksums remain in the release artifacts above.

Release note:
[`docs/releases/2026-07-18-coakka-client-1.3.1-2215b0f.md`](docs/releases/2026-07-18-coakka-client-1.3.1-2215b0f.md)

## coakka-runtime-inspect Public Artifacts

Current coakka-runtime-inspect source snapshots:

- macOS ARM64: `d7ab7fa`
- Linux ARM64: `4ce41f19`
- Linux x86_64: `4ce41f19`
- Windows x86_64: `d7ab7fa`
- Windows ARM64: `d7ab7fa`

Package contents:

- `runtime-inspect/native/releases/1.3.1+4ce41f19/coakka-runtime-inspect-v2-1.3.1-linux-aarch64.tar.gz`
- `runtime-inspect/native/releases/1.3.1+4ce41f19/coakka-runtime-inspect-v2-1.3.1-linux-x86_64.tar.gz`
- `runtime-inspect/native/releases/1.3.1+d7ab7fa/coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz`
- `runtime-inspect/native/releases/1.3.1+d7ab7fa/coakka-runtime-inspect-v2-1.3.1-windows-x86_64.tar.gz`
- `runtime-inspect/native/releases/1.3.1+d7ab7fa/coakka-runtime-inspect-v2-1.3.1-windows-aarch64.tar.gz`

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
[`docs/releases/2026-07-19-coakka-runtime-inspect-1.3.1-d7ab7fa-macos-aarch64.md`](docs/releases/2026-07-19-coakka-runtime-inspect-1.3.1-d7ab7fa-macos-aarch64.md),
[`docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-x86_64.md`](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-x86_64.md),
[`docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-aarch64.md`](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-4ce41f19-linux-aarch64.md),
and
[`docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-d7ab7fa-windows.md`](docs/releases/2026-07-20-coakka-runtime-inspect-1.3.1-d7ab7fa-windows.md)

## Runtime v2 Public Artifacts

Core source snapshot: `bda2ef5`

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
- `runtime/native/releases/1.3.1+bda2ef5/coakka-runtime-native-v2-1.3.1.tar.gz`

The staged native libraries include the runtime C ABI and scanner-clean
platform libraries for the public artifact surface. The host-facing routing
contract remains the same across profiles: targets, route generations,
request/reply, deadletters, and diagnostics stay in the public runtime
contract.

Runtime JVM now uses the `1.3.1-gbda2ef5-0a0aa76` connector refresh over native
package `1.3.1+bda2ef5`. Spring Boot and Quarkus adapters now use
`1.3.1-g0a0aa76` over that JVM runtime. Embedded-native coverage in the
published JVM jar now includes `macos-aarch64`, `linux-aarch64`,
`linux-x86_64`, `windows-aarch64`, and `windows-x86_64`. Python, Node.js, Go,
C#, Rust, Mojo, and Zig also publish against `1.3.1+bda2ef5` through release
directory `1.3.1+bda2ef5-0a0aa76`. Bun and the Tauri intent source package
publish against the same native runtime through release directory
`1.3.1+bda2ef5-247df1b`. Electron publishes against the same native runtime
through release directory `1.3.1+bda2ef5-4e0cab0`. Each release directory has
its own manifest and checksums.

Native refresh note:
[`docs/releases/2026-07-18-runtime-native-1.3.1-bda2ef5.md`](docs/releases/2026-07-18-runtime-native-1.3.1-bda2ef5.md)

Language connector release notes:
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
[`docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md`](docs/releases/2026-07-18-runtime-non-jvm-1.3.1-0a0aa76.md)

## Runtime Compatibility Matrix

These artifacts are the current public runtime set:

| Surface | Artifact | Version | Native package |
| --- | --- | --- | --- |
| Native C ABI | `runtime/native/releases/1.3.1+bda2ef5/coakka-runtime-native-v2-1.3.1.tar.gz` | `1.3.1+bda2ef5` | `1.3.1+bda2ef5` |
| JVM runtime | `coakka.v2:coakka-jvm-native-runtime-v2` | `1.3.1-gbda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Python runtime | `coakka_v2_connector` wheel | `1.3.1` from `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Node.js runtime | `coakka-v2-connector-node` package | `1.3.1` from `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Bun runtime | `coakka-v2-connector-bun` package | `1.3.1` from `1.3.1+bda2ef5-247df1b` | `1.3.1+bda2ef5` |
| Electron runtime | `coakka-v2-connector-electron` package | `1.3.1` from `1.3.1+bda2ef5-4e0cab0` | `1.3.1+bda2ef5` |
| Go runtime | `coakka-v2-connector-go` source package | `1.3.1` from `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| C# runtime | `CoAkka.Runtime` NuGet package | `1.3.1` from `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Rust runtime | `coakka-runtime-rs` package | `1.3.1-spike` from `1.3.1+bda2ef5-0a0aa76` | `1.3.1+bda2ef5` |
| Mojo runtime | `runtime/mojo/releases/1.3.1+bda2ef5-0a0aa76/coakka-runtime-mojo-1.3.1-source.tar.gz` | `1.3.1-source` | `1.3.1+bda2ef5` |
| Zig runtime | `runtime/zig/releases/1.3.1+bda2ef5-0a0aa76/coakka-runtime-zig-1.3.1-source.tar.gz` | `1.3.1-source` | `1.3.1+bda2ef5` |
| Tauri runtime | `runtime/tauri/releases/1.3.1+bda2ef5-247df1b/coakka-runtime-tauri-intents-1.3.1-source.tar.gz` | `1.3.1-source` | `1.3.1+bda2ef5` |
| Spring Boot adapter | `coakka.spring:coakka-spring-boot-starter` | `1.3.1-g0a0aa76` | via JVM runtime `1.3.1-gbda2ef5-0a0aa76` |
| Quarkus adapter | `coakka.quarkus:coakka-quarkus-extension` | `1.3.1-g0a0aa76` | via JVM runtime `1.3.1-gbda2ef5-0a0aa76` |

Do not mix runtime language packages from another native package generation
unless a later release note explicitly declares that combination compatible.

## Connector User Experience

The public connector onboarding contract is
[`docs/connector-user-experience-levels.md`](docs/connector-user-experience-levels.md).
It defines the intended progression for upcoming connector releases:
local handler/ask first, explicit route snapshots second, custom
envelope/transport policy last.

Release docs and samples must only present a helper as released after the
published artifact contains it. The runtime connector refresh in `0a0aa76`
keeps the local-first helper/doc level split introduced earlier for C#,
removes internal runtime fallback names from public resolvers, widens the
public package set to both Windows DLL arches, and keeps the current
source-package stance for Mojo/Zig; future connectors should keep matching
that level split instead of exposing route/endpoint internals in the first
practice path.

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
  --artifact /path/to/coakka_v2_connector-1.3.1-py3-none-any.whl \
  --expected-native-version 1.3.1+bda2ef5
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
