# CoAkka HTTP Runtime for Python

![CoAkka](https://raw.githubusercontent.com/phuong-tran/coakka-samples/a539dfcd7dc8d93c5b1c61b1324899af13d47ca6/docs/common/assets/brand/coakka-logo.png)

CoAkka HTTP Runtime provides HTTP server and client APIs for Python. Use it
to serve routes, stream responses, handle WebSockets, or call configured
upstream services. The wheel includes the native library for your platform.

## Install 2.0.1

Run these commands from your application directory with CPython 3.11 or newer.

On macOS or Linux:

```sh
python3 -m venv .venv
.venv/bin/python -m pip install coakka-http-runtime-python==2.0.1
```

On Windows in PowerShell:

```powershell
py -3 -m venv .venv
.\.venv\Scripts\python.exe -m pip install coakka-http-runtime-python==2.0.1
```

Install `coakka-http-runtime-python` with pip and import `coakka_http`
in your application.

## First request

Save this example as `server.py`.

```python
from coakka_http import Builder, Request, Response


def echo(request: Request) -> Response:
    return Response(status=201, body=request.body)


def main() -> None:
    service = Builder().listen("127.0.0.1", 8080).post("/echo", echo).start()
    with service:
        print(f"Listening on http://127.0.0.1:{service.port}")
        input("Press Enter to stop. ")


if __name__ == "__main__":
    main()
```

Run `.venv/bin/python server.py` on macOS or Linux, or
`.\.venv\Scripts\python.exe server.py` on Windows. From a second terminal,
send a request with the following command.

```sh
curl -i --data "hello" http://127.0.0.1:8080/echo
```

In Windows PowerShell, use `curl.exe`. The server returns HTTP 201 with
`hello` in the response body. Press Enter in the server terminal to shut
it down. The service context manager closes the runtime when the block exits.

## Requirements

| Platform | Architecture | Requirement |
| --- | --- | --- |
| macOS | ARM64 | macOS 11 or newer |
| Linux | x86-64 and ARM64 | glibc 2.28 or newer |
| Windows | x64 and ARM64 | CPython matching the wheel architecture |

If pip cannot find a matching wheel, run `python -VV` and
`python -c "import platform; print(platform.machine())"` with the interpreter
you plan to use. Install and run the package with that interpreter.

## Outbound requests

Keep the first server running. Save this example as `client.py` and run
it with the same virtual environment's Python. It sends a request to the
configured loopback endpoint and prints `201 hello`.

```python
from coakka_http import (
    Builder,
    OutboundEndpoint,
    OutboundReason,
    OutboundRequest,
    OutboundTarget,
)


def main() -> None:
    endpoint = OutboundEndpoint(
        node_id="local",
        connect_host="127.0.0.1",
        connect_port=8080,
        http_authority="127.0.0.1:8080",
    )
    target = OutboundTarget(name="echo", generation=1, endpoints=(endpoint,))

    with Builder().outbound_target(target).start() as service:
        call = service.outbound_submit(
            OutboundRequest("echo", "POST", "/echo", body=b"hello", timeout_ms=3000)
        )
        result = service.take_outbound(5000)
        if result is None:
            raise TimeoutError("No outbound result within five seconds")
        if result.call != call or result.reason is not OutboundReason.RESPONSE:
            raise RuntimeError(f"Outbound request ended with {result.reason.name}")
        print(result.response_status, result.response_body.decode("utf-8"))


if __name__ == "__main__":
    main()
```

`connect_host` selects the connection destination and `http_authority`
sets the HTTP authority. For TLS, configure the peer identity and trust
for the endpoint. A received HTTP error still arrives as a response with
its HTTP status. Use the terminal reason to distinguish transport and
lifecycle outcomes.

## API reference

- [Python integration guide](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/python/integration.md)
- [Runnable Python samples](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/python)
- [URL grammar and glossary](https://github.com/phuong-tran/coakka-samples/blob/main/docs/coakka-http-runtime/reference/glossary.md)
- [I/O backend](#io-backend)
- [Lower-level API](#lower-level-api)
- [Ownership and bounds](#ownership-and-bounds)

`Builder` creates one service. The service supports buffered and streamed
requests, streaming responses and trailers, SSE, WebSockets, static files,
outbound targets, route updates, and monitoring.

`service.routes` returns the runtime's route generation and binding
revisions. Use `effective_limits()` and `runtime_info()` to inspect the
configuration accepted by the runtime. Use
`Builder.compression(Compression(CompressionMode.GZIP))` to enable buffered
response compression.

## I/O backend

The service uses the platform's default I/O backend. On Linux, applications
can request io_uring explicitly.

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

The native runtime checks availability and uses the platform default when
the request cannot be applied. Read `io_uring_effective` to see the backend
in use and `fallback_reason` for the outcome of the request.

## Lower-level API

`Runtime` accepts one immutable `RuntimeConfig` and returns copied events.

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

[HTTP Runtime documentation](https://github.com/phuong-tran/coakka-samples/tree/main/docs/coakka-http-runtime)
