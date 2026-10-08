# CoAkka HTTP for Python

## Contents

- [Package status](#package-status)
- [Requirements](#requirements)
- [Buffered service](#buffered-service)
- [I/O backend](#io-backend)
- [Lower-level API](#lower-level-api)
- [Ownership and bounds](#ownership-and-bounds)
- [Verification](#verification)

## Package status

Five offline candidates are available in [candidates/2026-10-08-r3/](candidates/2026-10-08-r3/),
with `SHA256SUMS`: macOS ARM64, Linux ARM64/x86-64, and Windows ARM64/x86-64.
These are repository archives, not PyPI wheels or registry packages.

`coakka_http` is an idiomatic Python binding to the host-inlined CoAkka HTTP
runtime. It provides a bounded handler service and a lower-level typed event
API. The module uses only the focused public host interface; HTTP parsing, transport,
routing, files, TLS, outbound calls, and deadlines remain native-owned.

Each archive includes its matching native library. Wheel layout and PyPI
publication remain outside this step.

## Requirements

- CPython 3.11 syntax baseline; tested with 3.11.15 on macOS, 3.13.5 on
  Linux ARM64, and 3.12.10 on Linux x86-64 and both Windows targets.
- A matching OS/architecture archive. Linux and Windows x86-64 qualification
  used emulation; it is not physical x86-64 machine evidence.

Verify the archive against `SHA256SUMS`, extract it, and add its `python`
directory to `PYTHONPATH` before starting your application. For example, on
macOS ARM64, from this directory:

```sh
tar -xzf candidates/2026-10-08-r3/coakka-http-python-1.0.0-candidate-macos-aarch64.tar.gz
export PYTHONPATH="$PWD/coakka-http-python-1.0.0-candidate-macos-aarch64/python"
python3 your_application.py
```

The package selects its own paired library; no native-path environment variable
is needed. `COAKKA_HTTP_HOST_PATH` is an explicit development override only:
when supplied, it must name an absolute regular library file. Invalid or missing
package metadata fails rather than silently loading another installed library.

## Buffered service

```python
from coakka_http import Builder, Request, Response


def echo(request: Request) -> Response:
    return Response(status=201, body=request.body)


service = (
    Builder()
    .listen("127.0.0.1", 8080)
    .concurrency(2)
    .post("/echo", echo)
    .start()
)
try:
    print(service.port)
finally:
    service.close()
```

`Builder` is single-use. `Service` owns one native runtime, a sole inbound
reader, a fixed worker set, and bounded copied-request queues. Application
handlers never execute on the native event-loop thread. Header and handler
lookups are map-backed for average O(1) access while wire header order and
duplicates remain available.

The same surface supports buffered and incremental request bodies, response
streams and trailers, SSE, WebSocket, static mounts, confined file responses,
logical outbound targets, route rebinding/publication, health, liveness, route
snapshots, and bounded monitoring.

`service.routes` returns an immutable Core `RouteSnapshot`, including coherent
structural generation and binding revisions. It is not local handler declarations,
does not require monitoring, and raises on inspection failure rather than guessing.

`Builder.compression(Compression(CompressionMode.GZIP))` enables bounded buffered
compression; identity response streams remain independent and unbuffered.
Configure transport/handler deadlines through `Limits`; optional zero-valued limits
request Core defaults. Inspect accepted settings through `effective_limits()`
and `runtime_info()`, not the builder's input.

Outbound terminals carry typed `OutboundReason`, `OutboundPhase`, `OutboundRetry`
and `OutboundCertainty` facts. A received HTTP error response is still `RESPONSE`.
Unknown uint32 values retain their identity through `int(value)` without an
unbounded enum cache. Never automatically expose operator diagnostics to clients.

## I/O backend

Platform I/O is the default. On Linux that means epoll. io_uring requires an
explicit opt-in:

```python
from coakka_http import Builder, IoBackend, Response

service = (
    Builder()
    .io_backend(IoBackend.IO_URING)
    .get("/health", lambda _request: Response())
    .start()
)
try:
    info = service.runtime_info()
    print(info.io_uring_requested, info.io_uring_effective, info.fallback_reason)
finally:
    service.close()
```

Python only forwards this preference. Native startup owns capability checks,
kernel probing, protocol eligibility, initialization, and safe fallback to
epoll. The connector does not inspect `/proc`, compare kernel versions, or
issue a probe syscall. Benchmarks must require `io_uring_effective` before
labelling a result as io_uring.

## Lower-level API

`Runtime` accepts one immutable `RuntimeConfig` and returns copied events:

```python
from coakka_http import EventKind, Response, Route, Runtime, RuntimeConfig

configuration = RuntimeConfig(routes=(Route(41, "POST", "/items/{id}", 101),))
with Runtime(configuration) as runtime:
    runtime.start()
    event = runtime.take_event(5_000)
    if event is not None and event.kind is EventKind.REQUEST:
        runtime.respond(event.exchange, Response(status=202, body=b"accepted"))
```

Each blocking inbound, WebSocket, outbound, and monitor lane allows one reader.
All native views are copied during the call; no borrowed pointer escapes into
Python. Failures are `NativeError` values with a stable numeric status and a
bounded diagnostic.

## Ownership and bounds

Worker count, queued request count, retained bytes, stream items, handler
bindings, active exchanges, monitor pages, and shutdown time are finite.
Returning a streaming response gives one callback-scoped writer to its
producer. Pressure waits for native recovery credit and retries only the exact
rejected item.

`Service.close()` stops admission, wakes readers, joins connector workers, and
destroys native ownership only when no Python code can still reach it. If a
handler exceeds the deadline, close raises and retains the owner for a safe
retry. The operation is serialized and idempotent.

Monitoring is disabled by default. Enabling it reserves fixed storage during
construction. Signals are hints; callers pull immutable snapshots. Monitoring
pressure cannot block request handling or change an exchange result.

## Verification

From the repository root:

```bash
export COAKKA_HTTP_HOST_PATH=/absolute/path/to/libcoakka_http_host.dylib
export PYTHONPATH="$PWD/connectors/python"
python3 -m pytest -q connectors/python/tests
ruff check connectors/python/coakka_http connectors/python/tests
mypy --strict connectors/python/coakka_http
python3 connectors/python/scripts/verify-docstrings.py \
  connectors/python/coakka_http
```

The integration suite exercises real loopback traffic and covers default and
opt-in I/O selection, indexed headers, buffered/streamed requests and
responses, SSE, WebSocket, static/file delivery, outbound calls, route control,
monitoring, and lifecycle shutdown.

The October 8 candidates passed 100 tests and an independent three-cycle consumer
on each of the five targets listed above. Mac/Pi also passed real HTTP/2 and
HTTP/3 wire plus graceful-drain checks. These are exact-package checks, not a
full-interpreter sanitizer or benchmark claim.
