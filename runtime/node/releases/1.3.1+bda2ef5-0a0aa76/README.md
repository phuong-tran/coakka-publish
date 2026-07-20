# coakka-v2-connector-node

Node.js connector package for the CoAkka runtime v2.

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

Request/reply lane in Node.js now has two host API shapes over the same runtime contract:

- `ask...`: submit and wait inline
- `submitRequest...` + `terminalEvents(...)`: submit now, consume terminal outcome (`response` or `deadletter`) later through an async iterator

`terminalEvents(...)` is a connector-owned API shape, not a separate transport mode.

## Before / After

Before, local application code had to start from the connector-internal
orchestration name:

```ts
import {
  ConnectorOrchestrator,
  PayloadFormat,
  PayloadIdentity,
} from "@coakka/v2-connector-node";

const connector = ConnectorOrchestrator.start({
  systemName: "customer-local",
  nodeId: "customer-local-node",
  routes: [],
});
const requestIdentity = new PayloadIdentity(
  "customer.create.request.v1",
  1,
  PayloadFormat.JSON,
);

try {
  const response = await connector.askJson(
    "customer-api",
    "customer.create",
    { name: "Ada" },
    requestIdentity,
  );
} finally {
  connector.close();
}
```

After, the same single-process runtime is named as the application-owned host:

```ts
import {
  PayloadFormat,
  PayloadIdentity,
  RuntimeHost,
} from "@coakka/v2-connector-node";

const runtime = RuntimeHost.start({
  systemName: "customer-local",
  nodeId: "customer-local-node",
  routes: [],
});
const requestIdentity = new PayloadIdentity(
  "customer.create.request.v1",
  1,
  PayloadFormat.JSON,
);

try {
  const response = await runtime.askJson(
    "customer-api",
    "customer.create",
    { name: "Ada" },
    requestIdentity,
  );
} finally {
  runtime.close();
}
```

`ConnectorOrchestrator` remains available for existing code. New local-first
Spring Boot, Quarkus, CRUD, and script-style examples should prefer
`RuntimeHost` so the first screen reads as one embedded runtime owner, not a
remote connector setup.

`separateDeliveredRequestLane` defaults to `true`. Most request/reply services
should leave it alone so inbound handler work stays separate from
reply/deadletter matching. Set it to `false` only for advanced, measured,
mostly one-way hosts.

Hot-path reading note:

- false-sharing is not the first-order hot-path concern for this Node.js layer
  in the same way it is for the native C++ connector
- the current Node connector cost center is more likely to sit in:
  - `koffi` boundary and native read/write calls
  - wire codec encode/decode and JS object mapping
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

The build stages release-shaped native libraries from:

```text
v2/staging/native/1.3.1+bda2ef5/
```

Test:

```sh
npm test
```

Packaged consumer smoke:

```sh
npm run smoke:packaged
```
