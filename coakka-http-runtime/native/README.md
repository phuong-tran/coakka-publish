# CoAkka HTTP Runtime For Native C And C++

The native SDK is the direct, stable C and C++ entrypoint to CoAkka HTTP Runtime.
C and C++ share the same public API and ownership contract.

## Contents

- [Current Artifact](#current-artifact)
- [Install](#install)
- [How Native Fits](#how-native-fits)
- [Choose A Surface](#choose-a-surface)
- [Buffered C Service](#buffered-c-service)
- [Advanced API](#advanced-api)
- [Observability And Monitoring](#observability-and-monitoring)
- [TLS, mTLS, And Live Handler Changes](#tls-mtls-and-live-handler-changes)
- [Ownership And Shutdown](#ownership-and-shutdown)
- [Targets](#targets)
- [Verification](#verification)

## Current Artifact

Version `1.0.0` is staged privately as immutable release
`1.0.0+204d6ed28231ff9a39f4584393bf7d5af0b6975a`.

Each installed SDK tree contains the public C/C++ API, target-specific shared
library, build-system metadata, release metadata, SBOM, and usage/license
information.

## Install

Extract the private SDK for the exact target and point CMake at its prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/coakka-http-sdk
cmake --build build
```

Consume only the installed package and `<coakka/http/http.h>`. The target runtime
library is part of the SDK; applications do not need to build its dependencies.

## How Native Fits

Native C and C++ use the same runtime contract that every connector projects.
An application can choose the compact handler-based server or take direct
control of event lanes, streams, realtime sessions, pressure, health, and
monitoring without introducing a second HTTP engine.

Read [How It Works](../docs/how-it-works.md) for the request and pressure flows,
[App Host And Connectors](../docs/app-host-and-connectors.md) for ownership, and
[Frontend And Backend](../docs/frontend-and-backend.md) for the combined static,
API, SSE, and WebSocket application shape.

## Choose A Surface

The header exposes two public levels:

| Surface | Use it for |
| --- | --- |
| `coakka_http_server_*` | Direct buffered C/C++ services with callback handlers and finite worker/queue options |
| `coakka_http_core_*` | Advanced event API for streams, SSE, WebSocket, files, outbound HTTP, live route changes, health, and monitoring |

Both surfaces use copied configuration, finite limits, typed results, and
ordered shutdown. The handler-based server is the normal request/reply entry;
the event API exposes the complete operational contract used by language
packages.

## Buffered C Service

```c
#include <coakka/http/http.h>
#include <stdio.h>
#include <string.h>

static coakka_http_bytes_t bytes(const char *text) {
  coakka_http_bytes_t value = {
      .data = (const uint8_t *)text,
      .size = (uint64_t)strlen(text),
  };
  return value;
}

static void echo(void *context, coakka_http_request_t *request) {
  (void)context;
  coakka_http_response_t response;
  coakka_http_response_init(&response);
  response.status_code = 200;
  response.body = coakka_http_request_body(request);
  (void)coakka_http_request_respond(request, &response);
}

int main(void) {
  coakka_http_server_options_t options;
  coakka_http_route_t route;
  coakka_http_server_t *server = NULL;

  coakka_http_server_options_init(&options);
  options.bind_address = bytes("127.0.0.1");
  options.port = 8080;

  coakka_http_route_init(&route);
  route.route_id = 1;
  route.method = bytes("POST");
  route.path = bytes("/echo");
  route.handler = echo;

  coakka_http_result_t result =
      coakka_http_server_create(&options, &route, 1, &server);
  if (result.code != COAKKA_HTTP_RESULT_OK)
    return 1;
  result = coakka_http_server_start(server);
  if (result.code != COAKKA_HTTP_RESULT_OK) {
    (void)coakka_http_server_destroy(&server);
    return 1;
  }

  /* A production process replaces this with its owned signal/event loop. */
  (void)getchar();
  return coakka_http_server_destroy(&server).code == COAKKA_HTTP_RESULT_OK
             ? 0
             : 1;
}
```

The snippet shows API shape, not a complete process signal loop. Handler
contexts remain caller-owned through destroy. Request views are borrowed only
during the callback, and `request_respond` copies response bytes before a
successful return.

## Advanced API

The advanced API follows this shape:

1. initialize and populate `coakka_http_configuration_t`;
2. add listeners, routes, limits, file, and monitor policy;
3. create the service owner, then destroy the now-copied configuration;
4. start the service and run exactly one reader per required event lane;
5. respond, stream, accept WebSockets, rebind, or inspect health/monitor state;
6. release every leased event exactly once;
7. drain, interrupt readers, stop, and destroy.

Call `coakka_http_features()` before selecting an optional capability. The
`1.0.0` runtime surface exposes HTTP/1.1, feature-gated HTTP/2 and HTTP/3,
request/response streams, trailers, write timeouts, SSE, WebSocket, route
rebind, health, and monitoring. All five release targets execute static, SPA,
and application-file serving. See the [capability matrix](../docs/capabilities.md).

## Observability And Monitoring

The native runtime exposes non-blocking health, a fresh liveness probe, effective
monitor configuration, bounded aggregate snapshots, generation-checked live
policy, cursor-based recent events, missed-history counts, and one finite
wait/interrupt lane. Monitoring is disabled by default and never retains HTTP
payloads, credentials, cookies, or certificate material.

Read [Observability And Monitoring](../docs/observability-and-monitoring.md) for
the complete data model and operator lifecycle.

## TLS, mTLS, And Live Handler Changes

Listeners support plaintext, TLS, and mutual TLS with explicit credential
identity/generation, certificate chain, private key, and trust roots. Eligible
Linux HTTP/2 and HTTP/3 configurations can select the feature-gated
`io_uring` backend.

`coakka_http_core_rebind` atomically switches one existing route to a prepared
handler binding under expected route generation and binding revision. Existing
requests finish on the binding captured at admission. See
[TLS And mTLS](../docs/tls-and-mtls.md) and
[Live Handler Changes](../docs/handler-swap-and-hot-reload.md).

## Ownership And Shutdown

- configuration and successful response submissions copy borrowed input;
- a request/event lease keeps runtime storage alive until exact release;
- only one reader may own each inbound, WebSocket, outbound, or monitor lane;
- destroy must not overlap another call or an outstanding lease;
- pressure returns typed results such as `QUEUE_FULL` or `LIMIT_EXCEEDED`;
- all lifecycle waits use configured finite bounds and monotonic progress;
- stop closes admission and wakes readers before owner destruction.

The [operations guide](../docs/operations.md) documents queue and monitor
semantics shared with every connector.

## Targets

| Installed target | Library |
| --- | --- |
| macOS ARM64 | `libcoakka_http_runtime.1.0.0.dylib` |
| Linux ARM64 | `libcoakka_http_runtime.so.1.0.0` |
| Linux x86-64 | `libcoakka_http_runtime.so.1.0.0` |
| Windows ARM64 | `coakka_http_runtime.dll` plus import library |
| Windows x86-64 | `coakka_http_runtime.dll` plus import library |

Linux requires glibc 2.28 or newer. Windows uses the static MSVC runtime.
macOS x86-64 is not included.

## Verification

Use the [installed-package verification suite](../runtime-test/README.md)
to verify an installed tree through public package inputs. It configures the runtime,
starts a real loopback listener, drives an HTTP request, checks health and
monitoring, consumes terminal state, and performs idempotent shutdown.
