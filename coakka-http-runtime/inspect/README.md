# CoAkka HTTP Runtime Inspect

Optional standalone browser application for local HTTP service development.
Inspect connects to a service's explicitly enabled inspection endpoint, reads
route/schema snapshots, projects OpenAPI 3.0.3, and lets the developer try routes.
It is separate from CoAkka Runtime Inspect, which inspects runtime messaging.

## Contents

- [Packages and evidence](#packages-and-evidence)
- [Install](#install)
- [Connect to a service](#connect-to-a-service)
- [Security and lifecycle](#security-and-lifecycle)
- [Limitations](#limitations)

## Packages and evidence

These are qualified branch candidates, not registry releases. Keep the exact
archive identity when reporting a problem.

| Target | Archive | Bytes | Execution evidence |
| --- | --- | ---: | --- |
| macOS ARM64 | [Download](candidates/2026-10-08/coakka-http-runtime-inspect-1.0.0-macos-aarch64.tar.gz) | 3272697 | Installed application workflow on macOS ARM64 |
| Linux ARM64 | [Download](candidates/2026-10-08/coakka-http-runtime-inspect-1.0.0-linux-aarch64.tar.gz) | 4168385 | Rocky Linux ARM64, the same archive on Raspberry Pi 5 Debian 13, and an offline restricted Ubuntu 24.04 container |

Checksums: [SHA256SUMS](candidates/2026-10-08/SHA256SUMS).
The Linux package references system interfaces up to GLIBC 2.28. This is a
binary baseline, not certification of every distribution. macOS execution
evidence applies to the tested host; the package is ad-hoc signed, not notarized.
Windows and x86-64 Inspect packages are not included in this candidate set.

Recorded checks cover startup, health, browser assets, disconnected state,
connect/refusal, snapshot/OpenAPI, refresh, route try, disconnect/reconnect and
graceful process stop. Package checks cover exact contents, required license
texts, binary hardening and reproducible archive bytes. No throughput claim is
attached to the inspection application.

## Install

From the `coakka-publish` checkout, first verify both archived packages:

```sh
python3 scripts/verify-http-inspect-candidate.py
```

Extract only the package for your platform into a new application directory.
For example, on Linux ARM64:

```sh
mkdir http-inspect
tar -xzf coakka-http-runtime/inspect/candidates/2026-10-08/coakka-http-runtime-inspect-1.0.0-linux-aarch64.tar.gz -C http-inspect
./http-inspect/bin/coakka-http-runtime-inspect --version
./http-inspect/bin/coakka-http-runtime-inspect --help
```

Use the macOS archive instead on macOS ARM64. The archive contains one executable
and four required license/notice files under `share/licenses/`. It does not
require a separate HTTP Runtime library installation. Retain the license files.
Do not disable operating-system security controls to bypass a launch refusal.

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

## Limitations

This candidate is a local development tool, not a remotely exposed operations
console. It does not include publisher signing/notarization, a published
container image, Windows execution, or other CPU architectures. Those are
separate qualification scopes, not implied by the HTTP SDK platform matrix.

See the [HTTP Runtime catalog](../README.md) for service packages and
[operations guide](../docs/operations.md) for application lifecycle guidance.
