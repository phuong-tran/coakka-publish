# coakka-v2-connector

Python connector package for the CoAkka runtime v2.

Current package shape:

- `RuntimeHost.start(...)` as the preferred single-process lifecycle entrypoint
- `ConnectorOrchestrator.start(...)` remains as the compatibility name for the
  same runtime host
- `PythonRuntimeClient` as the lower-level request/reply engine
- `submit_request_typed(...)`, `submit_request_json(...)`, `submit_request_raw(...)`
- `terminal_events(buffer_capacity=...)`
- typed payload identity helpers around `message_type`, `payload_schema_version`, and `payload_format`
- control snapshot apply helpers
- monitor doorbell wait helpers
- optional separate delivered-request lane support
- one active runtime host per Python process

Request/reply lane in Python now has two host API shapes over the same runtime contract:

- `ask_*`: submit and wait inline
- `submit_request_*` + `terminal_events(...)`: submit now, consume terminal outcome (`response` or `deadletter`) later through a stdlib queue-backed subscription

`terminal_events(...)` is a connector-owned API shape, not a separate backend mode. `CAF` remains just the current transporter underneath.

## Before / After

Before, a local consumer had to learn the connector-internal orchestration name first:

```python
from coakka_v2_connector import (
    ConnectorOrchestrator,
    ConnectorStartSpec,
    PayloadFormat,
    PayloadIdentity,
)

start_spec = ConnectorStartSpec(
    system_name="customer-local",
    node_id="customer-local-node",
    routes=[],
)
request_identity = PayloadIdentity("customer.create.request.v1", 1, PayloadFormat.JSON)

with ConnectorOrchestrator.start(start_spec=start_spec) as connector:
    response = connector.ask_json(
        source="customer-api",
        target="customer.create",
        payload={"name": "Ada"},
        payload_identity=request_identity,
    )
```

After, the same single-process runtime is exposed with the name an application
owner expects:

```python
from coakka_v2_connector import (
    ConnectorStartSpec,
    PayloadFormat,
    PayloadIdentity,
    RuntimeHost,
)

start_spec = ConnectorStartSpec(
    system_name="customer-local",
    node_id="customer-local-node",
    routes=[],
)
request_identity = PayloadIdentity("customer.create.request.v1", 1, PayloadFormat.JSON)

with RuntimeHost.start(start_spec=start_spec) as runtime:
    response = runtime.ask_json(
        source="customer-api",
        target="customer.create",
        payload={"name": "Ada"},
        payload_identity=request_identity,
    )
```

`ConnectorOrchestrator` remains available for existing code. New local-first
Spring Boot, Quarkus, CRUD, and script-style examples should prefer
`RuntimeHost` so the file reads as one embedded runtime owner, not a remote
connector setup.

Hot-path reading note:

- false-sharing is not the first-order hot-path concern for this Python layer
  in the same way it is for the native C++ connector
- the current Python connector cost center is more likely to sit in:
  - `ctypes` boundary and native read/write calls
  - protobuf encode/decode and Python object mapping
  - queue/subscription churn around `terminal_events(...)`
  - thread and event handoff topology
- only revisit cacheline-style hardening here if this layer later moves toward
  packed native-side state, off-heap rings, or a flatter shared-memory layout

Local editable install from this repo:

```sh
python3 -m venv .venv-python-demo
source .venv-python-demo/bin/activate
pip install -e ./python
```

Runtime library resolution order:

1. explicit `runtime_lib_path` passed to `RuntimeHost.start(...)`
2. `$COAKKA_RUNTIME_LIB`
3. embedded wheel resource for the current platform
4. repo-local `lib/libcoakka_runtime_v2.{so|dylib}`

Current packaged native note:

- the wheel embeds `libcoakka_runtime_v2`
- native package version is `0.1.0+3362b67`
- current packaged platforms are `macos-aarch64`, `linux-aarch64`, and
  `linux-x86_64`

## Wheel Packaging

Fetch staged Linux natives if needed:

```sh
./python/scripts/fetch_linux_runtime_natives.sh
```

Build the wheel with the verified staged native set:

```sh
./python/scripts/build_wheel.sh
```

Run smoke from the packaged wheel:

```sh
./python/scripts/smoke_packaged_wheel.sh
```

Docs:

- [Consuming Guide](CONSUMING.md)
- [Release Checklist](RELEASE.md)
- [Standalone Consumer Smoke](consumer-smoke/README.md)

Minimal typed request example:

```python
from coakka_v2_connector import (
    ConnectorStartSpec,
    DeliveryHint,
    EndpointFlag,
    EndpointSpec,
    PayloadFormat,
    PayloadIdentity,
    RuntimeHost,
    RouteSpec,
)

request_identity = PayloadIdentity(
    message_type="demo.echo.request.v1",
    payload_schema_version=1,
    payload_format=PayloadFormat.JSON,
)

start_spec = ConnectorStartSpec(
    system_name="python-demo",
    node_id="python-demo-node",
    routes=[
        RouteSpec(
            target="svc.echo",
            endpoints=[
                EndpointSpec(host="127.0.0.1", port=19081, flags=int(EndpointFlag.LOCAL)),
            ],
        ),
    ],
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
        delivery_hint=DeliveryHint.ROUTER_DEFAULT,
    )
    print(response)
```
