# CoAkka HTTP Runtime for JavaScript

Status: GitHub release `1.0.0`. npm mirror not yet available.

The Node and Bun API uses `Request`, `Response`, `StreamResponse`,
`EventResponse`, `WebSocketSession`, `Service` and `Client`. Release `1.0.0`
includes buffered and streaming bodies, server events, WebSocket sessions,
static and SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and
multipart values, route updates, bounded inspection, cancellation and finite
shutdown.

The exact release artifact is under
[releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5](releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5/manifest.json).
The suffix identifies the exact Core and connector source snapshots; the
package version remains `1.0.0`. One deterministic tarball contains macOS
arm64, Linux arm64/x64 and Windows arm64/x64 payloads. Every target passed the
same installed real-socket capability suite on Node and Bun, plus integrity,
architecture, export and dependency checks. The x64 Linux and Windows
executions used emulation and are not physical-host performance evidence.

## Install From GitHub

Node:

```sh
npm install "https://raw.githubusercontent.com/phuong-tran/coakka-publish/coakka-http-runtime-v1.0.0/coakka-http-runtime/javascript/releases/1.0.0%2B3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5/coakka-http-1.0.0.tgz"
```

Bun:

```sh
bun add "https://raw.githubusercontent.com/phuong-tran/coakka-publish/coakka-http-runtime-v1.0.0/coakka-http-runtime/javascript/releases/1.0.0%2B3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5/coakka-http-1.0.0.tgz"
```

Both commands install `@coakka/http` version `1.0.0` from the same immutable
five-target package.

These files are the GitHub-hosted release. Additional dynamic, race, fault,
stress and soak coverage remains planned; npm publication is a separate future
distribution step.
