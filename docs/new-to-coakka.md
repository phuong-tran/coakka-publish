# New To CoAkka

CoAkka is a native-backed runtime and logger toolkit for application-owned
work. It helps an app route work by target name, handle request/reply,
deadletters, bounded queues, diagnostics, and native-backed logging without
turning every internal boundary into another hand-written HTTP endpoint.

## Two Public Repositories

| Repository | Use it for | Link |
| --- | --- | --- |
| `coakka-publish` | Released packages, native archives, manifests, checksums, compatibility matrix, and release notes. | https://github.com/phuong-tran/coakka-publish |
| `coakka-samples` | Runnable examples that show how an app uses the runtime and logger packages. | https://github.com/phuong-tran/coakka-samples |

If you are new, start with `coakka-samples`. Come back to `coakka-publish` when
you need exact artifact versions, checksums, compatibility rules, or release
history.

## What CoAkka Does

CoAkka gives application code a runtime boundary:

```text
caller code -> CoAkka target -> handler -> reply or deadletter
```

The target is a capability name such as `samples.customer.store` or
`billing.invoice.create`. A route snapshot says which process owns that target.
The runtime carries replies, route misses, timeout outcomes, counters, and
diagnostics through one vocabulary.

The logger is a separate product surface:

```text
app code -> bounded native logger -> drain/counters
```

It is useful when a sample or integration needs explicit queue capacity,
accepted/dropped counts, and native-backed delivery behavior instead of plain
console output.

## What CoAkka Is Not

CoAkka is not a Kafka clone, a hosted broker, a service mesh, a web framework,
or a replacement for public HTTP/gRPC APIs. It is an embedded runtime surface
used by an application host through language packages.

Public edges, authentication, deployment policy, service discovery, and product
APIs still belong to the application architecture around CoAkka.

## Current JavaScript Packages

The npm lane is published and registry-verified:

```sh
npm install coakka-v2-connector-node@1.3.6
npm install coakka-v2-connector-bun@1.3.6
npm install coakka-v2-connector-electron@1.3.6
npm install coakka-logger-node@1.2.4
npm install coakka-logger-bun@1.2.4
npm install coakka-logger-electron@1.2.4
```

The matching samples live in:

- https://github.com/phuong-tran/coakka-samples/tree/main/runtime/node
- https://github.com/phuong-tran/coakka-samples/tree/main/runtime/bun
- https://github.com/phuong-tran/coakka-samples/tree/main/runtime/electron
- https://github.com/phuong-tran/coakka-samples/tree/main/logger/node
- https://github.com/phuong-tran/coakka-samples/tree/main/logger/bun
- https://github.com/phuong-tran/coakka-samples/tree/main/logger/electron

The smallest no-checkout npm smoke is:

- https://github.com/phuong-tran/coakka-samples/blob/main/docs/first-npm-smoke.md

## First Learning Path

1. Clone and run samples:

   ```sh
   git clone https://github.com/phuong-tran/coakka-samples.git
   cd coakka-samples
   bash run.sh runtime node basic
   bash run.sh logger node basic
   ```

2. Read the compatibility matrix:

   https://github.com/phuong-tran/coakka-publish/blob/main/docs/compatibility-matrix.md

3. Check released artifacts and checksums only when you need exact distribution
   files:

   https://github.com/phuong-tran/coakka-publish
