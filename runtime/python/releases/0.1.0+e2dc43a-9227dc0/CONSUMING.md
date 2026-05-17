# Consuming `coakka-v2-connector`

## Build The Wheel

```sh
./python/scripts/build_wheel.sh
```

Expected wheel output:

```text
python/build/wheelhouse/
```

## Install From The Local Wheel

```sh
python3 -m venv .venv-coakka-python
source .venv-coakka-python/bin/activate
python -m pip install python/build/wheelhouse/coakka_v2_connector-0.1.0-py3-none-any.whl
```

## Native Loading

Default path:

- the installed package loads the embedded native runtime for the current platform
- the embedded native package version is `0.1.0+e2dc43a`
- current embedded platforms are `macos-aarch64`, `linux-aarch64`, and
  `linux-x86_64`

Override path:

```sh
export COAKKA_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.so
```

or on macOS:

```sh
export COAKKA_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.dylib
```

One Python process may start one active `RuntimeHost`. It owns the embedded
native runtime handle and must be closed during application shutdown.

## Minimal Example

```python
from coakka_v2_connector import ConnectorStartSpec, EndpointSpec, PayloadFormat, PayloadIdentity, RuntimeHost, RouteSpec

request_identity = PayloadIdentity(
    message_type="demo.echo.request.v1",
    payload_schema_version=1,
    payload_format=PayloadFormat.JSON,
)

start_spec = ConnectorStartSpec(
    system_name="python-demo",
    node_id="python-demo-node",
    routes=[RouteSpec(target="svc.echo", endpoints=[EndpointSpec(host="127.0.0.1", port=19081)])],
)

with RuntimeHost.start(start_spec=start_spec) as runtime:
    runtime.register_handler(
        "svc.echo",
        lambda request: runtime.client.make_json_reply_from_request_identity(
            request=request,
            source="svc.echo",
            payload={"echo": "ok"},
        ),
    )
    response = runtime.ask_json(
        source="test-client",
        target="svc.echo",
        payload={"message": "hello"},
        payload_identity=request_identity,
        timeout_ms=1000,
    )
    print(response)
```
