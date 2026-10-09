# CoAkka Documentation

CoAkka is a polyglot distributed runtime ecosystem. **CoAkka Runtime** is the
starting point for distributed target/message applications. **CoAkka HTTP
Runtime** is its independent HTTP server/client product; Logger and addons
have their own contracts and package channels.

This index links the existing documentation areas without moving them or
combining the products' configuration and installation instructions.

## Start with CoAkka Runtime

- [New to CoAkka](new-to-coakka.md) and [ecosystem overview](ecosystem-overview.md).
- [Runtime integration](runtime-integration-guide.md) and [current packages](current-packages.md).
- [Runtime logging and observability](runtime-logging-observability.md).
- [Runtime Inspect](coakka-runtime-inspect.md).
- [Runnable Runtime samples](https://github.com/phuong-tran/coakka-samples/tree/main/runtime).

## Build and operate HTTP Runtime

| Task | Guide |
| --- | --- |
| Understand the product | [Introduction and shared-Core benefits](coakka-http-runtime-introduction.md) |
| Choose a package | [HTTP product catalog](../coakka-http-runtime/README.md) |
| Run a first service | [Language integration guides](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/README.md#languages) |
| Learn routing and generations | [HTTP glossary](../coakka-http-runtime/docs/glossary.md) |
| Serve files and receive uploads | [File delivery and sendfile](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/file-delivery.md) |
| Monitor and export observations | [Kotlin-led monitoring guide](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/monitoring.md) |
| Manage shutdown, hooks and deployment | [Operations](../coakka-http-runtime/docs/operations.md), [without Kubernetes](../coakka-http-runtime/docs/deployment-without-kubernetes.md) |
| Configure TLS/mTLS and rotate certificates | [Transport security](../coakka-http-runtime/docs/tls-and-mtls.md) |
| Inspect routes, metadata and OpenAPI | [HTTP Runtime Inspect](../coakka-http-runtime/inspect/metadata-and-openapi.md) |
| Check measurements and upcoming work | [Benchmark results](../coakka-http-runtime/docs/benchmark-results-rpi5.md), [roadmap](../coakka-http-runtime/roadmap.md) |

HTTP Runtime ships through checksum-pinned repository archives today. Its
planned npm, Go-module, PyPI and Maven Central distribution is separate from
the existing CoAkka Runtime coordinates. HTTP Runtime Inspect is not the
distributed Runtime Inspect tool.

## Other ecosystem guides

- [Logger package entrypoints](current-packages.md#package-and-source-entrypoints).
- [Runtime addons](runtime-addons.md).
- [Complete sample documentation index](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md).
