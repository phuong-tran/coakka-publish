# CoAkka HTTP Runtime Inspect

Optional standalone browser application for local HTTP service development.
Inspect connects to a service's explicitly enabled inspection endpoint, reads
route/schema snapshots, projects OpenAPI 3.0.3, and lets the developer try routes.
It is separate from CoAkka Runtime Inspect, which inspects runtime messaging.

## Contents

- [Why Inspect exists, metadata, OpenAPI and Swagger UI](../../docs/coakka-http-runtime/inspect/metadata-and-openapi.md)

- [Packages and evidence](#packages-and-evidence)
- [Install](#install)
- [Connect to a service](#connect-to-a-service)
- [Security and lifecycle](#security-and-lifecycle)
- [Deployment scope](#deployment-scope)

## Packages and evidence

Inspect 2.0.1 embeds HTTP Runtime Core generation 2.0. Install a
matching standalone archive; you do not need to install the SDK separately
for Inspect.

| Target | Archive | Execution evidence |
| --- | --- | --- |
| macOS ARM64 | [Download](releases/2.0.1/coakka-http-runtime-inspect-2.0.1-macos-aarch64.tar.gz) | Installed workflow and [Node.js walkthrough](walkthrough.md) |
| Linux ARM64 | [Download](releases/2.0.1/coakka-http-runtime-inspect-2.0.1-linux-aarch64.tar.gz) | Installed workflow on Rocky Linux 8.10 ARM64 and Raspberry Pi 5 |

Compare your archive with [SHA256SUMS](releases/2.0.1/SHA256SUMS). The Linux
binary uses system interfaces up to glibc 2.28. The macOS executable has an
ad hoc signature and has not been notarized. Windows and x86-64 builds are
outside this release's scope.

The installed workflow checks startup, browser assets, connect and refusal,
OpenAPI export, route requests, disconnect and graceful stop. The
[original 1.0.0 candidate archives](candidates/2026-10-08/) remain available
for reproducing earlier runs.

## Install

From a `coakka-publish` checkout, verify and extract the archive for your
platform. This example uses Linux ARM64. On macOS ARM64, replace
`linux-aarch64` with `macos-aarch64` in `archive`.

```sh
archives="$PWD/coakka-http-runtime/inspect/releases/2.0.1"
archive=coakka-http-runtime-inspect-2.0.1-linux-aarch64.tar.gz
(cd "$archives" && grep "  $archive\$" SHA256SUMS | shasum -a 256 -c)
mkdir http-inspect
tar -xzf "$archives/$archive" -C http-inspect
./http-inspect/bin/coakka-http-runtime-inspect --version
```

Expect `coakka-http-runtime-inspect 2.0.1 assets=3`. The archive contains the
executable and its license files under `share/licenses/`. Keep those files
with the executable.

## Connect to a service

The target service must explicitly enable its authenticated inspection read
surface. An ordinary HTTP service, or a service with inspection disabled, is
not an inspection target. The bearer file must contain the token accepted by
that target: 32–4096 bytes of bearer-token text, without a trailing newline.
Provide an ordinary file, not a symbolic link, readable by the Inspect process;
restrict its permissions and do not commit or print its contents.

Start disconnected, with the path to that existing secret file:

```sh
./http-inspect/bin/coakka-http-runtime-inspect serve \
  --bearer-file /absolute/path/to/inspection-token \
  --listen-host 127.0.0.1 --listen-port 8080 --timeout-ms 3000
```

Open `http://127.0.0.1:8080` in a browser on the same machine. Enter the target's
numeric loopback host and inspection port, then choose Connect. Refresh reloads
the local snapshot; Disconnect leaves the browser application running.
Routes without business schema metadata remain routable but undocumented;
Inspect does not infer schemas from handlers or traffic.

For automation, provide `--target-host 127.0.0.1 --target-port PORT` to `serve`
for an initial connection, or obtain one snapshot without a browser listener:

```sh
./http-inspect/bin/coakka-http-runtime-inspect snapshot \
  --target-host 127.0.0.1 --target-port 9000 \
  --bearer-file /absolute/path/to/inspection-token --timeout-ms 3000
```

Replace `9000` with the service's inspection port. The timeout is a positive
request wait budget in milliseconds, not proof that a tried business operation
did not execute. Do not automatically retry state-changing operations.

## Security and lifecycle

- Use only on a trusted local development machine. The bearer authenticates
  target reads; it is not browser authentication. The browser listener has no
  remote-user authentication or TLS in this product scope.
- Both listener and target access are restricted to numeric loopback. Do not
  publish the listener through a proxy, port forward or container port mapping.
- A container's loopback is its own network namespace; it does not reach a
  service on the host merely because both use `127.0.0.1`.
- Route Try sends a real application request and can change application state.
  Application authorization remains the target application's responsibility.
- Browser reads use bounded cached snapshots. Refresh does not alter target
  routes, handlers or configuration. A failed refresh is not a successful new
  snapshot; check the displayed status before relying on the data.
- Stop with Ctrl+C or SIGTERM and wait for exit before restarting. Stopping
  Inspect does not stop the target service. Restart Inspect after changing its
  executable, bearer file or startup configuration.

## Deployment scope

Run Inspect on a trusted local development machine. The broader HTTP SDK
platform matrix does not extend to the standalone Inspect application.

See the [HTTP Runtime catalog](../README.md) for service packages and
[operations guide](../../docs/coakka-http-runtime/operations/operations.md) for application lifecycle guidance.
