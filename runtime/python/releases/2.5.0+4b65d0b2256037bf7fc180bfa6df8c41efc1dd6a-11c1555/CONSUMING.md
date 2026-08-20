# Consuming `coakka-v2-connector`

This is the Python connector for the polyglot, multi-language, multi-platform
CoAkka Runtime ecosystem.

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
python -m pip install python/build/wheelhouse/coakka_v2_connector-2.5.1-py3-none-any.whl
```

## Native Loading

Default path:

- the installed package loads the embedded native runtime for the current platform
- the bundled native generation is
  `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`
- the wheel includes `linux-aarch64`, `linux-x86_64`, `macos-aarch64`,
  `windows-aarch64`, and `windows-x86_64` native targets
- release-candidate wheel `2.5.1` request/reply passes on macOS ARM64 and Linux
  ARM64/x86-64
- the same 2.5.1 source passes Windows ARM64 after selecting `PeekNamedPipe`
  for CRT pipe readiness

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

Read [Transport Configuration](TRANSPORT_CONFIGURATION.md) before selecting a
non-default connection mode or TLS/mTLS. See the canonical
[troubleshooting guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for loader, capability, certificate, and platform-trust failures.

## Minimal Example

```python
from coakka_v2_connector import ConnectorStartSpec, PayloadIdentity, RuntimeHost, local_route

request_identity = PayloadIdentity.text("demo.echo.request.v1")

start_spec = ConnectorStartSpec(
    system_name="python-demo",
    node_id="python-demo-node",
    routes=[local_route("svc.echo")],
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
