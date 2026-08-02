# coakka-v2-connector

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

**This is the Python connector in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem.** CoAkka is not a Python-only
runtime: this package adapts Python applications to the same native core,
public C ABI, target, request/reply, bounded-admission, and deadletter contract
used by the JVM, Node.js, Go, C#, Rust, Swift, and other connector lanes.

Kubernetes is supported but not required. Use the public
[Ecosystem Overview](https://github.com/phuong-tran/coakka-publish/blob/main/docs/ecosystem-overview.md)
and [Compatibility Matrix](https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md)
to select the exact Python package and native OS/CPU payload.
Start with the [CoAkka Documentation](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
for concepts, integration paths, operations, and runnable samples.

Common guidance:

- [Connection strategies](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md)
- [TLS and mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
- [Troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
- [Contact and support](https://github.com/phuong-tran/coakka-publish/blob/main/docs/contact-and-support.md): `gabrielgun1983@gmail.com`
- [Python transport API](TRANSPORT_CONFIGURATION.md)

## New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. It helps an app route work by target name, handle request/reply,
deadletters, bounded queues, diagnostics, and native-backed logging without
turning every internal boundary into another hand-written HTTP endpoint.

Use these public repositories to orient first:

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-samples` | Runnable examples and code you can inspect first. | https://github.com/phuong-tran/coakka-samples |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |

Run the matching sample:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh runtime python basic
```

Python runtime sample docs:
https://github.com/phuong-tran/coakka-samples/tree/main/runtime/python

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

Try the PyPI package without cloning any CoAkka repo. The example uses the
same customer command that often becomes fake backend HTTP in a growing app:

```sh
python3 -m venv coakka-python-first-run
source coakka-python-first-run/bin/activate
python -m pip install coakka-v2-connector==1.3.6
```

```python
from coakka_v2_connector import (
    ConnectorStartSpec,
    DeliveryHint,
    PayloadFormat,
    PayloadIdentity,
    RuntimeHost,
    local_route,
)

target = "samples.customer.store.create"
store = {}

start_spec = ConnectorStartSpec(
    system_name="customer-app",
    node_id="customer-app-node-1",
    queue_capacity=64,
    strict_no_drop=True,
    generation=1,
    routes=[local_route(target, 19001)],
)

with RuntimeHost.start(start_spec=start_spec) as runtime:
    runtime.register_handler(
        target,
        lambda request: runtime.client.make_json_reply_from_request_identity(
            request=request,
            source=target,
            payload={"status": "created", "storedCount": len(store) + 1},
        ),
    )

    response = runtime.ask_json(
        source="customer-api",
        target=target,
        payload={"id": "cust-001", "name": "Ada Lovelace"},
        payload_identity=PayloadIdentity(
            "samples.customer.create.request.v1",
            1,
            PayloadFormat.JSON,
        ),
        timeout_ms=2000,
        operation="create_customer",
        delivery_hint=DeliveryHint.ROUTER_DEFAULT,
    )
    print(response)
```

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
- capability discovery before selecting optional connection or TLS features
- startup-configured connection strategy and TLS/mTLS policy
- structured atomic apply results and same-mode credential reload

Request/reply lane in Python now has two host API shapes over the same runtime contract:

- `ask_*`: submit and wait inline
- `submit_request_*` + `terminal_events(...)`: submit now, consume terminal outcome (`response` or `deadletter`) later through a stdlib queue-backed subscription

`terminal_events(...)` is a connector-owned API shape, not a separate backend mode. The current transport backend remains an implementation detail underneath.

## Before / After

CoAkka is easiest to understand from a fake backend HTTP handoff. The
browser/API edge can be real HTTP and should stay HTTP. The fake part is the
second private endpoint a team adds only so app-owned store work has something
URL-shaped to call:

```python
@app.post("/backend/customers")
def create_customer(command: CustomerDraft):
    return store.create(command)
```

The caller now owns URL config, HTTP parsing, status mapping, timeout mapping,
retry policy, logs, and test fixtures for a boundary that is not really a
public API:

```python
reply = requests.post(
    "http://customer-store/backend/customers",
    json=command,
    timeout=5,
)
reply.raise_for_status()
customer = reply.json()
```

Read the address change like this:

```text
Before fake backend HTTP:
  POST /api/customers -> requests.post("http://customer-store/backend/customers")

After CoAkka:
  POST /api/customers -> target "samples.customer.store" -> registered handler
```

After CoAkka, HTTP can stay at the real edge, and the private backend URL
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

Packaged native payload:

- release wheels embed `libcoakka_runtime_v2`; an explicit
  `COAKKA_RUNTIME_LIB` remains supported
- package metadata records native generation
  `1.4.0+2cee86bf`
- the package includes target-specific runtime libraries
  for macOS ARM64, Linux ARM64, and Windows x86-64
- the exact macOS ARM64 runtime has passed Python source tests and a host-only
  wheel consumer smoke
- the Linux and Windows payload formats and digests are verified; this package
  receipt makes no Python execution claim for those two targets

| Distribution target | Included in wheel | Package evidence |
| --- | --- | --- |
| macOS ARM64 | Yes | Exact runtime tests, TLS reload, host-only wheel smoke pass |
| Linux ARM64 | Yes | Runtime payload format and digest verified; no Python execution claim in this receipt |
| Windows x86-64 | Yes | Runtime payload verified under Windows 11 ARM64 compatibility; no Python execution claim in this receipt |

A platform-specific execution result does not imply execution on another
packaged target. Use [common troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
for OS/CPU selection, loader dependencies, digests, Gatekeeper, Authenticode,
and the explicitly documented absence of publisher signing.

## Wheel Packaging

Refresh Linux package inputs when maintaining the native payload:

```sh
./python/scripts/fetch_linux_runtime_natives.sh
```

Build the wheel from verified native inputs:

```sh
./python/scripts/build_wheel.sh
```

For a host-library consumer smoke:

```sh
COAKKA_V2_HOST_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.dylib \
  ./python/scripts/smoke_packaged_wheel.sh
```

Host-only output is package/consumer evidence for that host only; it is not
multi-platform execution evidence.

Run smoke from the packaged wheel:

```sh
./python/scripts/smoke_packaged_wheel.sh
```

Check package readiness:

```sh
./python/scripts/check_pypi_readiness.sh
```

Public docs:

- CoAkka samples: https://github.com/phuong-tran/coakka-samples
- Python runtime samples: https://github.com/phuong-tran/coakka-samples/tree/main/runtime/python
- Public artifacts and release notes: https://github.com/phuong-tran/coakka-publish
- Compatibility matrix: https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md

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
