# Public Artifact Contract

This document defines the public boundary for versioned artifacts distributed
from this repository.

## Current Surface

The current public release surface contains:

- logger packages and logger Maven artifacts
- runtime v2 public C ABI files under `include/` and `native/`
- runtime v2 native C ABI archive under `runtime/native/releases/`
- runtime connector packages under `runtime/{jvm,python,node,bun,electron,go,csharp,rust}/releases/`
- runtime source connector packages under `runtime/{mojo,zig,tauri}/releases/`
- independently versioned optional runtime addons under
  `runtime-addons/<addon>/native/releases/` once a verified archive is present
  in the root public artifact manifest
- runtime JVM, Spring Boot, and Quarkus artifacts under `maven/`
- coakka-client native CLI archives under `coakka-tools/coakka-client/releases/`
- coakka-client Docker Linux bundles under
  `coakka-tools/coakka-client/docker-demo/releases/`
- coakka-runtime-inspect native UI archives under
  `coakka-tools/coakka-runtime-inspect/releases/`

Android AAR candidates may be staged under `maven/android/releases/` and
mirrored into a Maven coordinate under `maven/coakka/v2/` for natural Gradle
evaluation. A resolvable candidate coordinate is distribution evidence, not
matching-host execution evidence. An AAR does not join the current supported
surface until it passes runtime intake, its exact device or emulator lifecycle
path is recorded, and its row is added to `artifacts/public-artifacts.tsv`.

Consumer-facing package downloads are listed in
`artifacts/public-artifacts.tsv`. Public samples should pin against that
manifest so sample metadata and publish contents do not drift silently. The
manifest is intentionally limited to the current public release surface:
`logger/*/releases/*`, `runtime/*/releases/*`, `coakka-tools/*/releases/*`,
`coakka-tools/*/*/releases/*`, `runtime-addons/*/native/releases/*/*`, and
selected Maven jars under `maven/coakka/`.
Older `cli/releases/`, `demo/coakka-client/releases/`, and
`runtime-inspect/native/releases/` paths remain as compatibility history, not
the current tool artifact surface.
Comment lines may record provenance for source-package lanes when the source
commit is part of the consumer-facing release identity.

Runtime language artifacts must pass
`scripts/verify-runtime-intake-artifact.py` before they are copied into a
release directory. That intake gate checks the bundled native package version,
rejects workspace/sample/test path leakage, rejects stale native library names,
and runs the recursive public surface scanner against the candidate archive.
Auxiliary artifacts that intentionally omit native libraries, such as JVM
sources jars, may pass the same gate with `--allow-no-native`; main runtime
connector packages must embed or resolve exactly one platform runtime library
for the current platform.

## Compatibility Rule

Runtime language and framework artifacts must declare which native runtime
package generation they consume. The publish README keeps the current
compatibility matrix. A new language package version may differ from the native
package version, but it must not silently drift to another native generation.

Public samples should treat the native package generation as the compatibility
anchor. If a sample pins a native package generation such as
`0.1.0+<git-commit>`, every runtime language/framework lane used by that sample
must either consume that same native package generation or document an explicit
compatibility exception.

Runtime addons version independently. Each addon release manifest must declare
runtime ABI major, minimum native runtime version, and required runtime
features. Matching addon and runtime version numbers are neither required nor
sufficient evidence of compatibility.

An addon archive must not embed another copy of CoAkka Runtime. It contains the
addon C ABI and native module; the consumer supplies one compatible runtime
package. Addon implementation dependencies must be statically absorbed where
licensing permits, and every declared platform must carry matching-host runtime
execution plus dynamic dependency audit evidence.

## Integrity Metadata

Every release directory must keep checksum metadata close to the artifact:

- `manifest.json`
- `SHA256SUMS`
- ecosystem checksum sidecars where applicable
- the root `artifacts/public-artifacts.tsv` row for consumer-facing downloads

Signature, SBOM, and attestation files are future release artifacts, not implicit
claims. When introduced, they should be checked by the same public surface gate
instead of living as separate manual notes.

## Distribution Channels

`coakka-publish` is a versioned artifact warehouse and catalog, not a single
product and not an aggregate release unit. It does not publish GitHub Releases.

The canonical distribution channels are immutable checked-in artifact paths,
raw GitHub URLs for those paths, the checked-in Maven layout, lane-local
manifests and checksums, `artifacts/public-artifacts.tsv`, and official package
registries. Git tags may record immutable intake or publication receipts, but a
tag does not create a repository-wide product version.

Registry-native channels are npm for JavaScript packages, PyPI for Python,
NuGet for C#, and the checked-in Maven layout for JVM coordinates. Official Go
and Swift source-package repositories use semantic Git tags and may keep
GitHub Release pages because those repositories are package-manager source
coordinates outside this warehouse.

Historical GitHub Release metadata that is not reproducible from a retained tag
is stored under `artifacts/release-receipts/<tag>/`. Receipt snapshots are
evidence only and never replace the root current-artifact index.

Package-manager lanes must not weaken the public artifact contract:

- package managers are distribution layers, not new runtime contracts
- language packages must still include or resolve the expected platform native
  library for the normal path
- end users must not have to install separate native implementation packages
- package-manager coordinates must be reflected in public docs and verification
  before samples present them as current
- no lane may require a `coakka-publish` GitHub Release attachment URL

Current package-manager coordinates and consumption rules are summarized in
[`package-manager-roadmap.md`](package-manager-roadmap.md).

## Consumer Shape

`coakka-client` archives are directly runnable CLI drops. They are published
under:

```text
coakka-tools/coakka-client/releases/<release-id>/coakka-client-v2-<version>-<platform>.tar.gz
```

The matching Docker Linux bundles are published under:

```text
coakka-tools/coakka-client/docker-demo/releases/<release-id>/coakka-client-docker-demo-v2-<version>-<platform>.tar.gz
```

`coakka-client` is a request/reply runtime client. It must not be documented as
an inspect/dashboard surface, topology authority, business schema registry, or
sink/business workflow owner.

Public runtime artifacts must be directly consumable in their target ecosystem.
For JVM, the downloaded jar is the artifact:

```text
runtime/jvm/releases/<release-dir>/coakka-jvm-native-runtime-v2-<version>.jar
```

That jar must embed the supported platform native runtime libraries under:

```text
native/<platform>/libcoakka_runtime_v2.so
native/<platform>/libcoakka_runtime_v2.dylib
native/<platform>/libcoakka_runtime_v2.dll
```

Consumers must not have to download a separate native runtime artifact for the
normal path.

Framework adapters such as `coakka.spring:coakka-spring-boot-starter` must
depend on the shared JVM runtime artifact. They must not embed or publish a
framework-specific native runtime library.

Java baseline metadata is part of the public contract for JVM artifacts:

- the plain runtime JVM connector jar is gated to Java 8 bytecode and declares
  `Automatic-Module-Name: coakka.v2.runtime`
- Spring Boot 3.x starter artifacts declare
  `Automatic-Module-Name: coakka.spring.boot.starter` and keep Java 17 as their
  baseline because Spring Boot 3 requires Java 17

## Connector UX Contract

Runtime connector packages must keep the first-run API smaller than the full
runtime model. The public onboarding progression is defined in
[`connector-user-experience-levels.md`](connector-user-experience-levels.md):

- Level 1: local handler and ask helpers for application users
- Level 2: explicit route snapshots for control planes and framework adapters
- Level 3: custom envelopes, delivery hints, and transport policy for advanced
  integrations

Release docs, consuming guides, and public samples must not expose route
snapshots, host/port placeholders, generated envelopes, or payload identity as
required first-run concepts when a released language helper can hide them.
Advanced APIs stay public, but beginner docs should not require users to learn
the runtime ABI before sending a local request.

Do not document a connector helper as released until the corresponding artifact
in this repository contains it. If a helper exists only in the source connector
repo, document it as an upcoming contract, not as a current release feature.

## Native Dependency Gate

Native runtime files are still verified before they are embedded into public
language artifacts.

Allowed external dependencies are only platform baseline libraries normally
present with the OS/runtime:

- Linux: libc/loader-level system libraries, and only narrowly justified C/C++
  runtime libraries if static linking is not available yet
- macOS: system frameworks and system libraries shipped with macOS

Homebrew, apt, source-checkout, or language-toolchain shared libraries are not
allowed as runtime requirements for public artifacts.

Release tooling must verify dynamic dependencies before publishing. The release
is blocked if a native runtime file requires any shared library outside the
platform baseline allowlist.

## Language Package Rule

Language packages may expose language-native APIs and language dependencies,
but they must not weaken the native runtime contract.

For JVM, Android, Python, Node.js, Bun, Electron, Go, C#, Rust, Mojo, Zig, and Tauri:

- the package may expose language-native APIs and language dependencies
- the package must include or resolve exactly one native runtime library for the
  current platform
- loading the native runtime must not require users to install extra native
  implementation packages
- public samples must use the language artifact directly

An Android AAR must also record its connector version, bundled native package
generation, native source commit, clean-source state, and included Android ABIs
in `assets/coakka/runtime-package.json`. Every declared ABI must contain both
`libcoakka_runtime_v2.so` and the thin `libcoakka_android_jni.so` bridge.

Mojo and Zig are currently published as source connector packages with bundled
native runtime libraries. They are public source packages, not package-manager
releases yet.

For native C/C++ packages:

- the package must expose the host-facing C ABI headers directly
- any C++ connector must be a thin wrapper over the C ABI, not a separate
  runtime contract
- supported platform native libraries must be included in the archive
- CMake examples must build against the archive without a separate source
  checkout
- C++ examples or wrappers must not require consumers to install implementation
  schema/transport dependencies; when in doubt, expose the C ABI only
- the archive is not a Maven artifact

Logger language packages follow the same end-user shape as runtime language
packages: the normal artifact must include or resolve the supported logger
native library for the current platform, and public samples must consume the
language artifact directly. Tauri logger artifacts may be source packages with
bundled logger natives until there is a package-manager release lane.

## Public Source Identity

Public artifacts and public samples must not mention unpublished source
checkout names or developer-machine paths.

Allowed wording:

- packaging source
- release input directory
- generated package directory

Not allowed in public docs, manifests, consuming guides, or sample imports:

- unpublished checkout names
- absolute developer-machine paths

Go packages need special care because the module path is public identity. A Go
release tarball must use a public, stable module path before the corresponding
sample is considered public-consumable. The current Go module lane satisfies
that rule with tagged public repositories for `coakka-runtime-go@v1.3.10` and
`coakka-logger-go@v1.2.5`.

For any future Go module path, keep public samples honest: consume the tarball
with a temporary local `replace` until the public module repository is tagged
and a clean consumer has verified `go get` without `replace`.
