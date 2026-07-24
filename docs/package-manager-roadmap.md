# Package Manager Roadmap

This page records the intended package-manager direction for the public CoAkka
artifact surface. It is a roadmap, not a current release claim.

Today, the canonical public distribution surface is:

- GitHub Release assets
- raw GitHub artifact URLs
- `artifacts/public-artifacts.tsv`
- release-local `manifest.json` and `SHA256SUMS`
- the checked-in Maven layout for JVM and framework artifacts

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
| 2 | Go modules | Go runtime and logger packages | Requires stable public module identity before release. |
| 3 | crates.io | Rust runtime/logger packages and Tauri host-side helpers | Should keep Rust as the native host boundary; do not present Tauri JavaScript as the runtime owner. |
| 4 | apt/deb | `coakka-client`, native tools, and possibly native development packages | Operational surface with signing, repository metadata, upgrade policy, and install/remove behavior. |

Other channels such as Homebrew, Scoop, Chocolatey, or NuGet gallery publishing
can be evaluated after the main language and Linux tooling lanes are stable.

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
   no `.proto` schema leakage, and no package dependencies on protobuf, libuv,
   CAF, FFI/native build helpers, or other implementation/native dependency
   packages.
3. Make Electron runtime/logger packages depend on the matching first-party
   Node package through npm registry version metadata instead of raw GitHub
   tarball URLs.
4. Add CI coverage for the package-manager artifact path or registry metadata.
5. Only after the registry artifact exists and CI verifies it, update the
   compatibility matrix and public samples from planned to current npm
   coordinates.

Until all of those gates pass, the existing GitHub Release tarballs remain
public artifacts, not npm package-manager release claims.

## Go Modules

Go packages need stable public module identity before release. A Go module
path is user-facing API, so it should not be treated as a throwaway archive
layout.

Before opening the Go module lane:

- choose and document the final public module path
- keep package names and import paths stable
- verify samples consume the module path a user would use
- keep native loading self-contained for the normal path

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
