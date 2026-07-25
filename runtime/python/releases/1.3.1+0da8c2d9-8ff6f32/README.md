# coakka-v2-connector

Python connector package for the CoAkka runtime v2.

Current package shape:

- `RuntimeHost.start(...)` as the preferred single-process lifecycle entrypoint
- `ConnectorOrchestrator.start(...)` remains as the compatibility name for the
  same runtime host
- `PythonRuntimeClient` as the lower-level request/reply engine
- `submit_request_typed(...)`, `submit_request_json(...)`, `submit_request_raw(...)`
- `terminal_events(buffer_capacity=...)`
- typed payload identity helpers around `message_type`,
  `payload_schema_version`, and `payload_format`, including
  `PayloadIdentity.text(...)`
- `local_route(...)` for same-process targets so first-run samples do not spell
  host/port placeholders or endpoint flag numbers by hand
- control snapshot apply helpers
- monitor doorbell wait helpers
- delivered-request lane enabled by default for request/reply hosts, with an
  advanced override for measured one-way-only hosts
- one active runtime host per Python process

Request/reply lane in Python now has two host API shapes over the same runtime contract:

- `ask_*`: submit and wait inline
- `submit_request_*` + `terminal_events(...)`: submit now, consume terminal outcome (`response` or `deadletter`) later through a stdlib queue-backed subscription

`terminal_events(...)` is a connector-owned API shape, not a separate backend mode. The current transport backend remains an implementation detail underneath.

## Before / After

CoAkka is easiest to understand from a fake backend HTTP handoff.

Before CoAkka, an app may keep a real browser/API HTTP edge, then add another
backend HTTP endpoint only to call work owned by the same app or team:

```python
@app.post("/backend/customers")
def create_customer(command: CustomerDraft):
    return store.create(command)
```

The caller now owns URL config, HTTP status mapping, timeout mapping, and test
fixtures for a boundary that is not really a public API:

```python
reply = requests.post(
    "http://customer-store/backend/customers",
    json=command,
    timeout=5,
)
reply.raise_for_status()
customer = reply.json()
```

After CoAkka, HTTP can stay at the real edge, and the internal capability
becomes a runtime target owned by the process that handles it:

```python
from coakka_v2_connector import (
    ConnectorStartSpec,
    PayloadFormat,
    PayloadIdentity,
    RuntimeHost,
    local_route,
)

start_spec = ConnectorStartSpec(
    system_name="customer-store",
    node_id="customer-store-node-1",
    routes=[local_route("samples.customer.store", 19102)],
)

def handle_customer(request):
    return runtime.client.make_json_reply_from_request_identity(
        request=request,
        source="samples.customer.store",
        payload={"status": "accepted"},
    )

with RuntimeHost.start(start_spec=start_spec) as runtime:
    runtime.register_handler("samples.customer.store", handle_customer)

    response = runtime.ask_json(
        source="samples.customer.frontend",
        target="samples.customer.store",
        payload={"name": "Ada"},
        payload_identity=PayloadIdentity(
            "samples.customer.create.request.v1",
            1,
            PayloadFormat.JSON,
        ),
        timeout_ms=5000,
        operation="create_customer",
    )
```

The important change is not "HTTP is bad." Public HTTP and gRPC still belong at
real API edges. CoAkka removes the extra backend HTTP surface when the caller
and handler are application-owned capabilities that only need typed runtime
delivery, request/reply completion, and deadletter visibility.

`ConnectorOrchestrator` remains available for existing code. New Python code
should prefer `RuntimeHost` so the file reads as one embedded runtime owner,
not a remote connector setup.

`separate_delivered_request_lane` defaults to `True`. Most request/reply
services should leave it alone so inbound handler work stays separate from
reply/deadletter matching. Set it to `False` only for advanced, measured,
mostly one-way hosts.

Hot-path reading note:

- false-sharing is not the first-order hot-path concern for this Python layer
  in the same way it is for the native C++ connector
- the current Python connector cost center is more likely to sit in:
  - `ctypes` boundary and native read/write calls
  - wire encode/decode and Python object mapping
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
- native package version is `1.3.6+0da8c2d9`
- current packaged platforms are `macos-aarch64`, `linux-aarch64`,
  `linux-x86_64`, `windows-aarch64`, and `windows-x86_64`

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
