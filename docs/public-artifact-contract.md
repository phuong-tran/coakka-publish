# Public Artifact Contract

This document defines the public boundary for artifacts released from this
repository.

## Current Surface

The current public release surface contains:

- logger packages and logger Maven artifacts
- runtime v2 public C ABI files under `include/` and `native/`
- runtime v2 native C ABI archive under `runtime/native/releases/`
- runtime connector packages under `runtime/{jvm,python,node,go,csharp,rust}/releases/`
- runtime JVM, Spring Boot, and Quarkus artifacts under `maven/`

Consumer-facing package downloads are listed in
`artifacts/public-artifacts.tsv`. Public samples should pin against that
manifest so sample metadata and publish contents do not drift silently. The
manifest is intentionally limited to the current public release surface:
`logger/*/releases/*`, `runtime/*/releases/*`, and selected Maven jars under
`maven/coakka/`.

Runtime language artifacts must pass
`scripts/verify-runtime-intake-artifact.py` before they are copied into a
release directory. That intake gate checks the bundled native package version,
rejects workspace/demo/test path leakage, rejects stale native library names,
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
anchor. If a sample pins `0.1.0+63c346e`, every runtime language/framework lane
used by that sample must either consume that same native package generation or
document an explicit compatibility exception.

## Integrity Metadata

Every release directory must keep checksum metadata close to the artifact:

- `manifest.json`
- `SHA256SUMS`
- ecosystem checksum sidecars where applicable
- the root `artifacts/public-artifacts.tsv` row for consumer-facing downloads

Signature, SBOM, and attestation files are future release artifacts, not implicit
claims. When introduced, they should be checked by the same public surface gate
instead of living as separate manual notes.

## Consumer Shape

Public runtime artifacts must be directly consumable in their target ecosystem.
For JVM, the downloaded jar is the artifact:

```text
runtime/jvm/releases/<release-dir>/coakka-jvm-native-runtime-v2-<version>.jar
```

That jar must embed the supported platform native runtime libraries under:

```text
native/<platform>/libcoakka_runtime_v2.so
native/<platform>/libcoakka_runtime_v2.dylib
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

For JVM, Python, Node.js, and Go:

- the package may expose language-native APIs and language dependencies
- the package must include or resolve exactly one native runtime library for the
  current platform
- loading the native runtime must not require users to install extra native
  implementation packages
- public samples must use the language artifact directly

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
sample is considered public-ready.
