# CoAkka HTTP Runtime

CoAkka HTTP Runtime is the CoAkka product surface for HTTP servers and clients.
Its APIs use the familiar Request, Response, Handler, Server and Client
concepts already used in everyday application code.

## Release Status

Version `1.0.0` is released from this GitHub repository. The exact artifacts,
manifests and checksums are stored below each language's `releases/1.0.0`
directory. They are not currently mirrored to npm, Maven Central, PyPI or the
Go module proxy.

Native C/C++, JVM, Python, JavaScript and Go are included in this release. The
public contracts use Request/Response vocabulary; implementation and build
contracts are not part of this repository surface.

## Language Plans

| Language | Status | Release |
| --- | --- | --- |
| Native C/C++ | ABI 1 with five verified targets | [native](native/README.md) |
| JVM / Java / Kotlin | Complete capability surface with five verified targets | [jvm](jvm/README.md) |
| Python | Complete capability surface with five verified targets | [python](python/README.md) |
| JavaScript / Node / Bun | Complete capability surface with five verified targets | [javascript](javascript/README.md) |
| Go | Complete capability surface with five verified targets | [go](go/README.md) |

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

The test source is available for review in this repository and can be run
against each matching native release artifact.

## Release Plan

1. Add more matching-host sanitizer, pressure, fault, stress and soak evidence.
2. Expand runnable consumer samples around the exact GitHub artifacts.
3. Mirror packages to language registries through a separate release decision.

GitHub is the distribution channel for `1.0.0`; future registry publication
will mirror the same language surfaces.
