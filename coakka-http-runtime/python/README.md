# CoAkka HTTP Runtime for Python

Status: GitHub release `1.0.0`. PyPI mirror not yet available.

The Python API uses `Request`, `Response`, `StreamResponse`, `EventResponse`,
`WebSocketSession`, `Service` and `HttpClient`. Release `1.0.0` includes
buffered and streaming bodies, server events, WebSocket sessions, static and
SPA delivery, outbound requests, TLS, HTTP/1.1, HTTP/2, form and multipart
values, route updates, bounded inspection, cancellation and finite shutdown.

The exact release artifacts are under
[releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5](releases/1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5/manifest.json)
for macOS arm64, Linux arm64, Linux x86_64, Windows arm64 and Windows x86_64.
The suffix identifies the exact Core and connector source snapshots; the
package version remains `1.0.0`. Every target passed the same installed
real-socket capability suite, wheel integrity and embedded Core identity
checks. The x86_64 Linux and Windows executions used emulation and are not
physical-host performance evidence.

## Install From GitHub

Download the wheel for the current machine from the release directory, then
install it into a Python 3.11 or newer environment:

```sh
python -m pip install ./coakka_http-1.0.0-py3-none-<platform>.whl
```

| Platform | Wheel suffix |
| --- | --- |
| macOS arm64 | `macosx_11_0_arm64` |
| Linux arm64 | `linux_aarch64` |
| Linux x86_64 | `linux_x86_64` |
| Windows arm64 | `win_arm64` |
| Windows x86_64 | `win_amd64` |

The exact wheel URL is also accepted by `pip`; pin the immutable
`coakka-http-runtime-v1.0.0` tag rather than the moving branch.

These files are the GitHub-hosted release. Broader Python 3.11, sanitizer,
race, fault, stress and soak coverage remains planned; PyPI publication is a
separate future distribution step.
