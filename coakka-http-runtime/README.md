# CoAkka HTTP Runtime

CoAkka HTTP Runtime is the upcoming CoAkka product surface for HTTP servers and
clients. Its API uses the familiar Request, Response, Handler, Server and Client
concepts already used in everyday application code.

## Development Status

This directory is a GitHub-level development draft. It is not a released
package and is not available from npm, Maven Central or PyPI. API details,
package names and platform coverage may change before the first release.

The first reviewable package is the [native C/C++ draft](native/README.md). It
contains one public C header and one closed shared library per target. The
public contract is Request/Response only; private runtime and build contracts
are not part of this repository surface.

## Language Plans

| Language | Status | Draft |
| --- | --- | --- |
| Native C/C++ | ABI 1 development draft with verified artifacts | [native](native/README.md) |
| JVM / Java / Kotlin | In development; publication follows the native baseline | [jvm](jvm/README.md) |
| JavaScript | Planned after the native baseline | [javascript](javascript/README.md) |
| Go | Planned after the native baseline | [go](go/README.md) |
| Python | Planned after the native baseline | [python](python/README.md) |

Each connector is planned to expose the same supported HTTP capabilities in a
form natural to its language. Connector publication remains closed until the
native contract is stable and the language package has equivalent lifecycle,
bounds and capability evidence.

## Native Test

The source-visible [runtime-test](runtime-test/README.md) is a strict C11/C++20
black-box consumer. It builds only from an installed native package and tests
real Request/Response traffic, lifecycle, limits, invalid calls and concurrent
requests without runtime source or private headers.

## Release Plan

1. Review and stabilize the native Request/Response ABI.
2. Extend the native surface only with fully bounded, tested capabilities.
3. Complete each language connector against the same supported capabilities.
4. Add runnable consumer samples after an exact package is approved.
5. Publish to registries only through a separate release decision.

Until those gates close, everything in this directory remains a development
draft. Artifact presence does not imply a registry release or production
support claim.
