# Package Manager Roadmap

This page records the package-manager direction for the public CoAkka artifact
surface. npm, PyPI, Go modules, SwiftPM, and NuGet are current; the remaining
channels are still roadmap items until their registry or repository paths are
published and verified.

Today, the canonical public distribution surface is:

- GitHub Release assets
- raw GitHub artifact URLs
- `artifacts/public-artifacts.tsv`
- release-local `manifest.json` and `SHA256SUMS`
- the checked-in Maven layout for JVM and framework artifacts
- npm registry packages for the Node.js, Bun, and Electron runtime/logger
  lanes
- PyPI packages for Python runtime/logger lanes
- public Go modules for Go runtime/logger lanes
- public SwiftPM packages for Swift runtime/logger lanes
- NuGet package for the C# runtime lane

Package managers should improve installation ergonomics without changing the
runtime boundary or native dependency contract.

## Rule

Package-manager publishing is distribution only.

It must not make implementation dependencies part of the user contract.
Consumers must not have to install native implementation packages outside the
published artifact path. Runtime and logger packages should keep the same
public shape they have today: language-native API at the top, bundled or
resolved platform native library underneath, and no extra native setup for the
normal path.

## Intended Order

| Priority | Channel | Main use | Notes |
| --- | --- | --- | --- |
| 1 | npm | Node.js, Bun, Electron runtime and logger packages | Best first package-manager lane because it improves JavaScript and desktop onboarding together. |
| 2 | PyPI | Python runtime and logger wheels | Runtime package `coakka-v2-connector==1.3.4` and logger package `coakka-logger==1.2.2` are published and install-smoked. |
| 3 | Go modules | Go runtime and logger packages | Runtime `github.com/phuong-tran/coakka-runtime-go@v1.3.10` and logger `github.com/phuong-tran/coakka-logger-go@v1.2.5` are published and clean-consumer verified. |
| 4 | SwiftPM | Swift runtime and logger source packages | Current for macOS ARM64; runtime `github.com/phuong-tran/coakka-runtime-swift@1.3.2`, logger `github.com/phuong-tran/coakka-logger-swift@1.2.1`. |
| 5 | NuGet | C#/.NET runtime and logger packages | Runtime `CoAkka.Runtime==1.3.3` is published and clean-consumer verified. Logger remains artifact-mirror only. |
| 6 | crates.io | Rust runtime/logger packages and Tauri host-side helpers | Should keep Rust as the native host boundary; do not present Tauri JavaScript as the runtime owner. |
| 7 | apt/deb | `coakka-client`, native tools, and possibly native development packages | Operational surface with signing, repository metadata, upgrade policy, and install/remove behavior. |

Other channels such as Homebrew, Scoop, or Chocolatey can be evaluated after
the main language and Linux tooling lanes are stable.

## npm

The first npm lane should focus on packages that already have public artifact
shape:

- `coakka-v2-connector-node`
- `coakka-v2-connector-bun`
- `coakka-v2-connector-electron`
- `coakka-logger-node`
- `coakka-logger-bun`
- `coakka-logger-electron`

The npm package may contain JavaScript/TypeScript entrypoints and package
metadata, but it must still preserve the native dependency rule. Bun can
consume npm packages, so a well-shaped npm release can serve both Node.js and
Bun users where the package API is compatible.

Electron packages should keep the renderer boundary explicit: renderer code
sends intent through preload/IPC, and the Electron main process owns runtime or
logger execution.

### Bootstrap Milestones

The npm lane should open in small gates:

1. Add a package-manager preflight that audits the current Node.js, Bun, and
   Electron runtime/logger tarballs without changing their planned status.
2. Make Node.js and Bun runtime/logger packages registry-ready: no
   `private=true`, explicit license carry-through, no install-time native setup,
   no generated wire schema files, and no implementation dependency packages in
   user-facing package metadata.
3. Make Electron runtime/logger packages depend on the matching first-party
   Node package through npm registry version metadata instead of raw GitHub
   tarball URLs.
4. Add CI coverage for the package-manager artifact path or registry metadata.
5. Only after the registry artifact exists and CI verifies it, update the
   compatibility matrix and public samples from planned to current npm
   coordinates.

Those gates have passed for the npm lane below. GitHub Release tarballs remain
the canonical non-npm artifact surface for every other lane.

### Current Runtime npm Lane

The runtime npm package set published from
`package-manager/npm/candidates/b46f705/` is current for the native payload
bridge release over runtime native `1.3.2+caff6d6d`:

- `coakka-v2-connector-node@1.3.9`
- `coakka-v2-connector-bun@1.3.9`
- `coakka-v2-connector-electron@1.3.9`

The current published logger npm lane remains:

- `coakka-logger-node@1.2.6`
- `coakka-logger-bun@1.2.6`
- `coakka-logger-electron@1.2.6`

The publish helper keeps the first-party dependency order explicit:

```sh
scripts/publish-npm-package-manager-candidates.sh --dry-run
scripts/publish-npm-package-manager-candidates.sh --publish --otp <current-code>
scripts/verify-npm-registry-candidates.sh --manifest package-manager/npm/candidates/b46f705/manifest.json
```

Public JavaScript samples consume the `1.3.9` runtime coordinates after the
registry verification gate passes.

## PyPI

Python is the current PyPI package-manager lane after npm. The runtime and
logger wheels are published on PyPI and remain mirrored from the GitHub
artifact surface.

Current wheel coordinates:

- runtime package name: `coakka-v2-connector`
- runtime import name: `coakka_v2_connector`
- runtime wheel artifact version: `1.3.2`
- logger package name: `coakka-logger`
- logger import name: `coakka_logger`
- logger wheel artifact version: `1.2.2`

The current runtime and logger wheels are published on PyPI and remain mirrored
as GitHub Release artifacts:

- PyPI current: `coakka-v2-connector==1.3.4`
- PyPI current: `coakka-logger==1.2.2`
- PyPI yanked runtime versions: `coakka-v2-connector==1.3.2`,
  `coakka-v2-connector==1.3.3`
- PyPI yanked logger version: `coakka-logger==1.2.1`
- GitHub Release artifact mirror: `runtime/python/releases/1.3.2+caff6d6d-6d5ea58/coakka_v2_connector-1.3.2-py3-none-any.whl`
- GitHub Release artifact mirror: `logger/python/releases/1.2.1+f50756ebff0d/coakka_logger-1.2.2-py3-none-any.whl`

The yanked PyPI versions were superseded because their public package
metadata/onboarding links were not the final public shape. They remain version
history; new installs should use the current versions above.

Verified PyPI logger gate:

- package name and version availability were checked before first upload
- the logger PyPI readiness gate passed in the connector release workspace
- PyPI upload succeeded for `coakka-logger==1.2.2`
- clean install smoke loaded logger native package `1.2.1+f50756ebff0d`
- public logger samples now install from PyPI

Current readiness gates:

```sh
./python/scripts/check_pypi_readiness.sh
./logger/python/scripts/check_pypi_readiness.sh
```

## Go Modules

Go is a current package-manager lane. The runtime/logger Go packages are
published as public modules with stable module paths:

- `github.com/phuong-tran/coakka-runtime-go@v1.3.10`
- `github.com/phuong-tran/coakka-logger-go@v1.2.5`

Current public samples consume those module coordinates directly. The GitHub
Release tarballs remain valid artifact-mirror drops for provenance and
compatibility, but they are no longer the normal Go install path.

Current module repositories:

- `https://github.com/phuong-tran/coakka-runtime-go`, tag `v1.3.10`
- `https://github.com/phuong-tran/coakka-logger-go`, tag `v1.2.5`

The release module export commands are:

```sh
# From the connector release workspace:
CONNECTOR_ROOT="$(pwd)"

cd "${CONNECTOR_ROOT}/go"
bash scripts/export-module-repo.sh /tmp/coakka-runtime-go-module
cd /tmp/coakka-runtime-go-module
go test ./...

cd "${CONNECTOR_ROOT}/logger/go"
bash scripts/export-module-repo.sh /tmp/coakka-logger-go-module
cd /tmp/coakka-logger-go-module
go test ./...
```

The tarball lane remains valid as the artifact mirror:

- runtime tarball:
  `runtime/go/releases/1.3.2+caff6d6d-6d5ea58/coakka-v2-connector-go-1.3.2.tar.gz`
- logger tarball:
  `logger/go/releases/1.2.1+f50756ebff0d/coakka-logger-go-1.2.1.tar.gz`

## SwiftPM

SwiftPM is the current package-manager lane for Swift runtime/logger
connectors. The package shape is source-first and macOS ARM64 only:

- runtime: `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact `1.3.2`, native runtime `1.3.2+caff6d6d`
- logger: `https://github.com/phuong-tran/coakka-logger-swift.git`, exact `1.2.1`, native logger `1.2.1+f50756ebff0d`

Current public repositories:

- `https://github.com/phuong-tran/coakka-runtime-swift`, tag `v1.3.2`
- `https://github.com/phuong-tran/coakka-logger-swift`, tag `v1.2.1`

The release manifest is tracked in `package-manager/swiftpm/current.json`.
Public samples consume those Git tags directly.

## NuGet

NuGet is current for the C# runtime connector:

- runtime package: `CoAkka.Runtime`
- runtime version: `1.3.3`
- native runtime generation: `1.3.2+caff6d6d`
- package URL: `https://www.nuget.org/packages/CoAkka.Runtime/1.3.3`

Clean-consumer verification installs from `https://api.nuget.org/v3/index.json`
and starts a local runtime host through the package RID native asset path.

The C# logger remains an artifact-mirror package in
`logger/csharp/releases/1.2.1+f50756ebff0d/` until a NuGet logger release gate
is opened.

## crates.io

Rust package publishing should support backend and desktop host developers
without weakening the host boundary.

For Tauri, the correct shape is still:

```text
WebView JavaScript intent -> Rust command boundary -> CoAkka runtime/logger -> result
```

A Rust crate can make the host-side command bridge easier to wire, but it
should not imply that frontend JavaScript owns the runtime. A Tauri-specific
plugin may be considered later only if it preserves that boundary.

## apt/deb

The apt/deb lane should come after the language package-manager lanes because
it is an operator-facing distribution surface, not just a package upload.

Good apt/deb candidates:

- `coakka-client`
- `coakka-runtime-inspect`
- native development headers and libraries, if a Linux dev package becomes
  useful

Before opening apt/deb:

- define repository signing and key rotation
- define package names and upgrade behavior
- define install/remove scripts
- define supported Linux distributions and architectures
- keep CLI/native tool artifacts aligned with the GitHub Release and manifest
  surface

## Release Gate

A package-manager lane is not public until:

- the artifact exists in the package manager or repository
- the same generation is represented in `artifacts/public-artifacts.tsv` or an
  equivalent checked release manifest
- the compatibility matrix names the package-manager coordinate
- public samples consume the package-manager path or clearly document that the
  lane is still GitHub Release based
- CI verifies the package-manager artifact or repository metadata

Until then, docs should describe the channel as planned, not current.
