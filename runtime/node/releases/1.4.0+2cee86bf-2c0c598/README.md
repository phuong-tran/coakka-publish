# coakka-v2-connector-node

<p align="center">
  <img src="https://raw.githubusercontent.com/phuong-tran/coakka-samples/main/docs/assets/brand/coakka-logo.png" alt="CoAkka" width="480">
</p>

**This is the Node.js connector in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem.** CoAkka is not a Node.js-only
runtime: this package adapts Node applications to the same native core, public
C ABI, target, request/reply, bounded-admission, and deadletter contract used
by the JVM, Python, Go, C#, Rust, Swift, and other connector lanes.

Kubernetes is supported but not required. Node applications can use this
connector on any OS/CPU tuple listed for the exact package release, including
standalone hosts, containers, VMs, and architecture-matched edge deployments.
See the public [Ecosystem Overview](https://github.com/phuong-tran/coakka-publish/blob/main/docs/ecosystem-overview.md)
and [Compatibility Matrix](https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md).
Start with the [CoAkka Documentation](https://github.com/phuong-tran/coakka-samples/blob/main/docs/README.md)
for concepts, integration paths, operations, and runnable samples.

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
bash run.sh runtime node basic
```

No-checkout npm smoke:
https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

Samples docs directory:
https://github.com/phuong-tran/coakka-samples/tree/main/docs

Try the npm package without cloning any CoAkka repo. The example uses the same
customer command that often becomes fake backend HTTP in a growing app:

```sh
mkdir coakka-runtime-first-run
cd coakka-runtime-first-run
npm init -y
npm install coakka-v2-connector-node
```

```js
import {
  DeliveryHint,
  localRoute,
  NodeRuntimeClient,
  PayloadFormat,
  PayloadIdentity,
  RuntimeHost,
} from "coakka-v2-connector-node";

const target = "samples.customer.store.create";
const store = new Map();

const runtime = RuntimeHost.start({
  systemName: "customer-app",
  nodeId: "customer-app-node-1",
  queueCapacity: 64,
  strictNoDrop: true,
  generation: 1,
  routes: [localRoute(target, 19001)],
});

try {
  runtime.registerHandler(target, (request) => {
    const draft = JSON.parse(Buffer.from(request.payload).toString("utf8"));
    const customer = { id: draft.id, name: draft.name, createdBy: request.source };
    store.set(customer.id, customer);

    return NodeRuntimeClient.makeJsonReplyFromRequestIdentity(request, target, {
      status: "created",
      customer,
      storedCount: store.size,
    });
  });

  const response = await runtime.askJson(
    "customer-api",
    target,
    { id: "cust-001", name: "Ada Lovelace" },
    new PayloadIdentity("samples.customer.create.request.v1", 1, PayloadFormat.JSON),
    2000,
    "create_customer",
    DeliveryHint.ROUTER_DEFAULT,
  );
  console.log(response);
} finally {
  runtime.close();
}
```

Current package shape:

- `RuntimeHost.start(...)` as the preferred single-process lifecycle entrypoint
- `ConnectorOrchestrator.start(...)` remains as the compatibility name for the
  same runtime host
- `NodeRuntimeClient` as the lower-level request/reply engine
- `submitRequestTyped(...)`, `submitRequestJson(...)`, `submitRequestRaw(...)`
- `terminalEvents({ signal, bufferCapacity })`
- typed payload identity helpers around `messageType`, `payloadSchemaVersion`,
  and `payloadFormat`, including `PayloadIdentity.text(...)`
- `localRoute(...)` for same-process targets so first-run samples do not spell
  host/port placeholders or endpoint flag numbers by hand
- control snapshot apply helpers
- monitor doorbell wait helpers
- delivered-request lane enabled by default for request/reply hosts, with an
  advanced override for measured one-way-only hosts
- capability discovery, startup-configured connection strategy, and atomic
  TLS/mTLS generation reload with structured results

See [Connection Strategies](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md),
[TLS and mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md),
and [Troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md).

Request/reply lane in Node.js now has two host API shapes over the same runtime contract:

- `ask...`: submit and wait inline
- `submitRequest...` + `terminalEvents(...)`: submit now, consume terminal outcome (`response` or `deadletter`) later through an async iterator

`terminalEvents(...)` is a connector-owned API shape, not a separate transport mode.

## Before / After

Before, the browser/API edge can be real HTTP, but teams often add a second
private backend HTTP endpoint only so work owned by the same app or team has
an address:

```js
app.post("/backend/customers", async (req, res) => {
  const customer = await store.create(req.body);
  res.json({ status: "created", customer });
});

app.post("/api/customers", async (req, res) => {
  const reply = await fetch("http://customer-store/backend/customers", {
    method: "POST",
    headers: { "content-type": "application/json" },
    body: JSON.stringify(req.body),
  });

  res.json(await reply.json());
});
```

After, the public API can stay HTTP, but the fake backend URL becomes a CoAkka
target:

```js
app.post("/api/customers", async (req, res) => {
  const response = await runtime.askJson(
    "customer-api",
    "samples.customer.store.create",
    req.body,
    new PayloadIdentity("samples.customer.create.request.v1", 1, PayloadFormat.JSON),
    5000,
    "create_customer",
    DeliveryHint.ROUTER_DEFAULT,
  );

  res.json(response);
});
```

The change is not "replace HTTP." HTTP still belongs at real browser/API or
legacy edges. CoAkka removes backend HTTP that exists only to call capabilities
owned by the same app or team by URL.

`ConnectorOrchestrator` remains available for existing code. New examples
prefer `RuntimeHost` so the first screen reads as one embedded runtime owner,
not a remote connector setup.

`separateDeliveredRequestLane` defaults to `true`. Most request/reply services
should leave it alone so inbound handler work stays separate from
reply/deadletter matching. Set it to `false` only for advanced, measured,
mostly one-way hosts.

Hot-path reading note:

- false-sharing is not the first-order hot-path concern for this Node.js layer
  in the same way it is for the native C++ connector
- the current Node connector cost center is more likely to sit in:
  - native binding boundary and runtime read/write calls
  - internal transport framing and JS object mapping
  - async iterator buffering around `terminalEvents(...)`
  - event-loop and worker handoff topology
- only revisit cacheline-style hardening here if this layer later moves toward
  packed native-side state, off-heap rings, or a flatter shared-memory layout

Install dependencies for local development:

```sh
cd node
npm install
```

Build:

```sh
npm run build
```

The repository build resolves and verifies the exact native generation recorded
by package metadata.

Test:

```sh
npm test
```

Packaged consumer smoke:

```sh
npm run smoke:packaged
```

For a host-library consumer smoke:

```sh
COAKKA_V2_HOST_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.dylib \
  npm run smoke:packaged
```

This smoke proves only the selected host/library tuple. Use the public
compatibility matrix for packaged platform coverage. Support contact:
`gabrielgun1983@gmail.com`.
