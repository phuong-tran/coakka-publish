# PyPI Package Lane

PyPI is the public package-manager lane for the Python CoAkka runtime and
logger connectors. Both packages load an architecture-matched native library;
their Python package version and bundled native generation are separate
identities.

## Current Coordinates

| Surface | Install | Native generation |
| --- | --- | --- |
| Runtime | `python -m pip install coakka-v2-connector==2.1.0` | `2.1.0+60ddf70d` |
| Logger | `python -m pip install coakka-logger==1.2.2` | `1.2.1+f50756ebff0d` |

The machine-readable release receipt is [`current.json`](current.json).
Superseded versions are yanked rather than deleted; pin the versions above.

## Runtime 2.1.0

PyPI and the artifact mirror contain `coakka_v2_connector-2.1.0-py3-none-any.whl`
from connector source `34d15e7` over native generation `2.1.0+60ddf70d`. The wheel
contains Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64 libraries,
passes package and digest gates, and completes a packaged request/reply smoke
on macOS ARM64. It also contains the Windows `PeekNamedPipe` waiter correction
and the file-lane API. Its PyPI JSON SHA-256 matches the staged wheel exactly,
and a clean registry wheel completes request/reply on macOS ARM64.

## Runtime Platforms And Evidence

The runtime wheel contains native payloads for Linux ARM64/x86-64, macOS
ARM64, and Windows ARM64/x86-64. All five payloads pass package, binary-format,
architecture, and digest verification.

Exact PyPI `2.1.0` request/reply executes on macOS ARM64. The wheel contains the
`PeekNamedPipe` waiter correction and all five platform payloads. Cross-platform
execution claims remain limited to the evidence recorded for each host.

See [Runtime Package And Platform Evidence](../../docs/runtime-package-platform-evidence.md)
for the exact distinction between published, contained, verified, executed,
and source-fixed states.

## Runtime First Run

Use the complete lifecycle: start one process-owned host, register the target,
send an identified request, handle reply/deadletter/timeout, and close the host.
The public sample keeps those semantics visible:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh runtime python basic
```

The sample prints connector-facing runtime version `2.1.0` and native git
identity `60ddf70d63b94750bb76c8284923e73199788c2e` before the response and
counters. The registry `2.1.0` wheel exposes file-lane.

## Verification

Release intake checks the PyPI JSON version, filename, yanked state, and
SHA-256, then installs from `https://pypi.org/simple` in a clean environment.
Payload presence is not used as a substitute for matching-host execution.

For connector APIs and runnable Python code, use the public
[`coakka-v2-connector` artifact README](../../runtime/python/releases/2.1.0+60ddf70d-34d15e7/README.md)
and the [Python samples](https://github.com/phuong-tran/coakka-samples/tree/main/runtime/python).
For loader, architecture, certificate, and publisher-trust failures, use
[Troubleshooting](../../docs/troubleshooting.md).
