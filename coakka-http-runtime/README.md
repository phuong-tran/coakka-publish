# CoAkka HTTP Runtime

CoAkka HTTP Runtime is the upcoming CoAkka product surface for HTTP servers and
clients. Its API uses the familiar Request, Response, Handler, Server and Client
concepts already used in everyday application code.

## Development Status

This directory is a GitHub-level development draft. It is not a released
package and is not available from npm, Maven
Central or PyPI. API details, package names and platform coverage may change
before the first release.

The native C/C++, JVM, Python, JavaScript and Go drafts are available for review.
The public contracts use Request/Response vocabulary; private runtime and build
contracts are not part of this repository surface.

## Language Plans

| Language | Status | Draft |
| --- | --- | --- |
| Native C/C++ | ABI 1 development draft with verified artifacts | [native](native/README.md) |
| JVM / Java / Kotlin | Complete capability draft with five verified targets | [jvm](jvm/README.md) |
| Python | Complete capability draft with five verified targets | [python](python/README.md) |
| JavaScript / Node / Bun | Complete capability draft with five verified targets | [javascript](javascript/README.md) |
| Go | Complete capability draft with five verified targets | [go](go/README.md) |

Each connector is expected to present the same HTTP capabilities in a form
natural to its language: buffered and streaming bodies, asynchronous handlers,
server events, WebSocket sessions, static and SPA delivery, outbound requests,
TLS, supported HTTP protocol selection, form and multipart values, route
updates, bounded inspection, cancellation and finite shutdown.

## Native Test

The source-visible [runtime-test](runtime-test/README.md) is a strict C11
black-box harness for the installed CoAkka HTTP Core ABI. It checks route
contract identity, borrowing, bounds, failure classification and concurrent
independent calls without access to runtime source.

The test source is available for review at the GitHub repository level and can
be run against each matching native draft artifact.

## Release Plan

1. Complete the language-native API and capability tests for each connector.
2. Rebuild and run matching-host package verification for every advertised
   operating system and architecture.
3. Complete sanitizer, pressure, fault, stress and soak evidence.
4. Add runnable consumer samples only after an exact package is approved.
5. Publish to package registries only through a separate release decision.

Until those gates close, this directory is roadmap and development-status
documentation. It must not be used as evidence that a registry package or
production support claim exists.
