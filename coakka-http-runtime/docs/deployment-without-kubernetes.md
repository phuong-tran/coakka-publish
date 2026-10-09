# Deployment without Kubernetes

Kubernetes is not required to run CoAkka HTTP Runtime. Each application can
run as an ordinary supervised process on a physical machine or VM. However,
the current HTTP package does **not** provide autonomous cluster membership,
leader election, cross-node state replication or a cluster scheduler.
Running several instances is a deployment pattern, not a built-in clustering
feature. This guide describes responsibilities, not a tested turnkey cluster.

## Contents

- [Ownership](#ownership)
- [A small deployment](#a-small-deployment)
- [Deploy and drain](#deploy-and-drain)
- [Monitor every instance](#monitor-every-instance)
- [What autonomous clustering would require](#what-autonomous-clustering-would-require)

## Ownership

| Concern | Owner |
| --- | --- |
| Per-instance HTTP I/O, bounds, timeouts and lifecycle | HTTP Runtime Core |
| Handlers and application state | App Host and application |
| Starting, restarting and resource-limiting processes | Deployment supervisor |
| Choosing healthy upstream instances and public ingress | External load balancer / reverse proxy |
| Service inventory, rollout ordering and configuration distribution | Deployment tooling or operator |
| Durable state, sessions, jobs and cross-node coordination | Explicit application services, not HTTP Core |
| Multi-instance dashboards and alerts | Monitoring backend and per-instance adapters |

The separate CoAkka Runtime product's distributed messaging model must not be
read as an implicit HTTP-cluster feature. Composing the two requires explicit
application integration and its own deployment contract.

## A small deployment

```text
Clients -> load balancer -> application A + HTTP Runtime
                        -> application B + HTTP Runtime

Each machine: supervisor owns its processes
Each application: monitor adapter -> shared monitoring backend
```

Use distinct addresses or ports for replicas. A single machine can host
multiple processes, but losing that machine loses all of them. Multiple
machines do not by themselves remove a single load balancer or shared database
as a failure point. Plan availability for those dependencies separately.

Keep public ingress separate from protected operations endpoints. The
application projects readiness from real service and dependency conditions;
do not expose internal diagnostics directly. The load balancer should stop
sending new work to an unready instance. A failed business dependency and a
stuck process need not have the same restart policy.

Do not assume a shared port, automatic discovery, synchronized handler swaps,
shared sessions or replicated files. Deploy compatible route/configuration and
asset versions to each instance deliberately. Keep application state external
or define explicit affinity, persistence and failover semantics. Long-lived
WebSocket/SSE connections stay attached to their selected instance; they do
not migrate automatically when it stops.

## Deploy and drain

1. Start a replacement instance using the selected verified package and config.
2. Check its effective settings, readiness and application dependencies before
   adding it to the upstream set.
3. Remove the old instance from new-traffic selection and allow the ingress
   change to take effect; account for persistent upstream connections.
4. Invoke the language service's documented graceful shutdown path. Let
   accepted work settle within its configured bounds. Long-lived sessions need
   an application reconnect policy and a finite drain deadline.
5. Stop application-owned monitor/export workers in lifecycle order and confirm
   close completed before reclaiming resources or restarting that owner.

If drain fails or reaches its deadline, record the typed outcome and follow
the operator's explicit escalation policy; a forced process kill is not a
successful graceful shutdown. Use bounded restart backoff rather than a crash
loop. Test ingress removal, slow requests, disconnect/reconnect, dependency
failure and machine loss in the chosen deployment before claiming availability.

## Monitor every instance

Tag exported data with bounded service/environment/instance identity and
preserve collection epochs, resets and missing-history indicators. Aggregate
compatible counters and histogram buckets; do not average instance p99 values
and label the result a cluster p99. Track exporter loss separately from HTTP
failure. Monitoring a replica does not make it a cluster coordinator.

See the [Kotlin-led monitor guide](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/monitoring.md)
for the shared source/adapter/sink model and language source recipes.

## What autonomous clustering would require

If the product is to own a cluster without an external control plane, that is
a separate design and implementation scope: membership/discovery, authenticated
node identity, placement, health/failure detection, network-partition behavior,
configuration consistency, rollout/rollback and state ownership all need
explicit contracts and fault tests. Where a leader or replicated state is
required, its election and consistency guarantees must be designed, not assumed.
It cannot be supplied by adding a README switch or relabeling local CPU loops.
