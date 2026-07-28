# Go Connector

The Go runtime connector package lives under `go/`.

Quick verification:

```bash
cd go
go test ./...
```

The live runtime integration smoke can be enabled with:

```bash
export COAKKA_GO_INTEGRATION=1
cd go
go test ./...
```

This integration lane runs a helper subprocess to avoid `dlopen` collisions inside the `go test` binary on macOS.

Package smoke with the embedded native runtime:

```bash
cd go
bash scripts/smoke-packaged-package.sh
```

Package release tarball:

```bash
cd go
bash scripts/package-release.sh
```

Archive output:

```text
go/coakka-v2-connector-go-1.3.1.tar.gz
```

Main public surface:

- `StartRuntimeHost(startSpec, runtimeLibPath)` as the preferred single-process
  lifecycle entrypoint
- `StartConnectorOrchestrator(startSpec, runtimeLibPath)`
- `GoRuntimeClient`
- `PayloadIdentity`
- `NewTextPayloadIdentity(...)` and `PayloadFormatText` for text-first samples
- `LocalRouteDefault(...)` / `LocalRoute(...)` for same-process targets
- `SubmitRequestTyped(...)`, `SubmitRequestJSON(...)`, `SubmitRequestRaw(...)`
- `TerminalEvents(ctx, buffer)`
- `MakeJSONReplyFromRequestIdentity(...)`
- `RuntimeControlClient`
- `RuntimeMonitor`
- delivered-request lane enabled by default for request/reply hosts; set
  `DisableSeparateDeliveredRequestLane: true` only for advanced, measured,
  mostly one-way hosts

## Before / After

Before, a local consumer had to start from an orchestration-shaped API name:

```go
startSpec := coakka.ConnectorStartSpec{
	SystemName: "customer-local",
	NodeID:     "customer-local-node",
	Routes:     nil,
}
requestIdentity := coakka.NewPayloadIdentity(
	"customer.create.request.v1",
	1,
	coakka.PayloadFormatJSON,
)

connector, err := coakka.StartConnectorOrchestrator(startSpec, "")
if err != nil {
	return err
}
defer connector.Close()

response, err := connector.AskJSON(
	"customer-api",
	"customer.create",
	map[string]any{"name": "Ada"},
	requestIdentity,
	2*time.Second,
	"create",
	coakka.DeliveryHintRouterDefault,
	nil,
)
```

After, it is still the same single-process runtime, but the entrypoint reads as an application-owned runtime host:

```go
startSpec := coakka.ConnectorStartSpec{
	SystemName: "customer-local",
	NodeID:     "customer-local-node",
	Routes:     nil,
}
requestIdentity := coakka.NewPayloadIdentity(
	"customer.create.request.v1",
	1,
	coakka.PayloadFormatJSON,
)

runtime, err := coakka.StartRuntimeHost(startSpec, "")
if err != nil {
	return err
}
defer runtime.Close()

response, err := runtime.AskJSON(
	"customer-api",
	"customer.create",
	map[string]any{"name": "Ada"},
	requestIdentity,
	2*time.Second,
	"create",
	coakka.DeliveryHintRouterDefault,
	nil,
)
```

`StartConnectorOrchestrator` remains available for existing code. New local-first code should use `StartRuntimeHost` so readers can see immediately that the current process owns an embedded runtime host, not a remote or Kubernetes setup.

Native runtime resolution order:

- explicit `runtimeLibPath`
- `$COAKKA_RUNTIME_LIB`
- packaged native under `native/<platform>/`
- local fallback under `lib/`

The Go request/reply lane currently has two host API shapes on the same runtime contract:

- `Ask...`: submit and wait inline
- `SubmitRequest...` + `TerminalEvents(...)`: submit first, then receive the terminal outcome (`response` or `deadletter`) through a channel

`TerminalEvents(...)` is a connector-owned API shape, not a separate transport mode.

Hot-path reading note:

- false-sharing is not currently the first-order hot-path concern in this Go layer in the same way it can be for a native C++ connector
- the more likely cost centers are:
  - `cgo` boundary and native read/write calls
  - protobuf marshal/unmarshal
  - channel/subscriber churn around `TerminalEvents(...)`
  - goroutine handoff topology
- revisit cacheline/padding hardening only if this layer later moves toward packed shared state, off-heap rings, or layouts that are more cacheline-sensitive

Cross-language demo web lives under `examples/` when that workspace is present.
