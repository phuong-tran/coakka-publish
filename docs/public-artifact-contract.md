# Public Artifact Contract

This document defines the public boundary for artifacts released from this
repository.

## Current Surface

The current public release surface contains:

- logger packages and logger Maven artifacts
- sanitized direct runtime v2 C ABI files under `include/` and `native/`
- sanitized runtime v2 native C ABI archive under `runtime/native/releases/`

Consumer-facing package downloads are listed in
`artifacts/public-artifacts.tsv`. Public samples should pin against that
manifest so sample metadata and publish contents do not drift silently. The
manifest is intentionally limited to the current public release surface:
`logger/*/releases/*` and `runtime/native/releases/*`.

Runtime JVM, language connector, Spring Boot, and Quarkus package lanes remain
paused until they are rebuilt against the sanitized runtime surface and pass the
gates below. The rules in this document remain the contract for reopening those
lanes.

Candidate runtime language artifacts must pass
`scripts/verify-runtime-intake-artifact.py` before they are copied into a
release directory. That intake gate checks the bundled native package version,
rejects workspace/demo/test path leakage, rejects stale native library names,
and runs the recursive public surface scanner against the candidate archive.
Auxiliary artifacts that intentionally omit native libraries, such as JVM
sources jars, may pass the same gate with `--allow-no-native`; main runtime
connector packages must embed or resolve exactly one platform runtime library
for the current platform.

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
- CMake examples must build against the archive without a private source
  checkout
- C++ examples or wrappers must not require consumers to install implementation
  schema/transport dependencies; when in doubt, expose the C ABI only
- the archive is not a Maven artifact

## Public Source Identity

Public artifacts and public samples must not mention private source workspace
names or private repository paths.

Allowed wording:

- source workspace
- packaging source
- release input directory
- generated package directory

Not allowed in public docs, manifests, consuming guides, or sample imports:

- local private checkout names
- private repository names
- absolute developer-machine paths

Go packages need special care because the module path is public identity. A Go
release tarball must use a public, stable module path before the corresponding
sample is considered public-ready.
