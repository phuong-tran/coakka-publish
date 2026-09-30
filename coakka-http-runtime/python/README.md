# CoAkka HTTP for Python

`coakka_http` is an idiomatic Python binding to the host-inlined CoAkka HTTP
runtime. It provides a bounded handler service and a lower-level typed event
API. The module uses only the focused public host interface; HTTP parsing, transport,
routing, files, TLS, outbound calls, and deadlines remain native-owned.

The connector does not bundle a shared library. Wheel layout and PyPI
publication are intentionally outside this development slice.

## Requirements

- CPython 3.11 or newer;
- an installed CoAkka HTTP host shared library;
- `COAKKA_HTTP_HOST_PATH` set to that absolute regular-file path during local
  development, or the library available through the platform loader.

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
