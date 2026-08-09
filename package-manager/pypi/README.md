# PyPI Package Lane

PyPI is the public package-manager lane for the Python CoAkka runtime and
logger connectors. Both packages load an architecture-matched native library;
their Python package version and bundled native generation are separate
identities.

## Current Coordinates

| Surface | Install | Native generation |
| --- | --- | --- |
| Runtime | `python -m pip install coakka-v2-connector==1.4.6` | `1.4.1+9e02a51d` |
| Logger | `python -m pip install coakka-logger==1.2.2` | `1.2.1+f50756ebff0d` |

The machine-readable release receipt is [`current.json`](current.json).
Superseded versions are yanked rather than deleted; pin the versions above.

## Runtime 2.1.0 Artifact

The artifact mirror contains `coakka_v2_connector-2.1.0-py3-none-any.whl` from
connector source `4782dcd` over native generation `2.1.0+60ddf70d`. The wheel
contains Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64 libraries,
passes package and digest gates, and completes a packaged request/reply smoke
on macOS ARM64. It also contains the Windows `PeekNamedPipe` waiter correction
and the file-lane API. It is not a PyPI coordinate until upload and a clean
install from `https://pypi.org/simple` are verified.

## Runtime Platforms And Evidence

The runtime wheel contains native payloads for Linux ARM64/x86-64, macOS
ARM64, and Windows ARM64/x86-64. All five payloads pass package, binary-format,
architecture, and digest verification.

Exact PyPI `1.4.6` request/reply executes on macOS ARM64 and Linux
ARM64/x86-64. On Windows 11 ARM64, the wheel loads native runtime `1.4.1`, but
its published reader uses `select()` with CRT pipe descriptors and times out
before receiving the reply. Current connector source uses `PeekNamedPipe` and
passes the same Windows ARM64 request/reply smoke; that fix is not part of the
immutable `1.4.6` wheel. Windows x86-64 connector execution is not claimed.

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

The sample prints both connector-facing runtime version `1.4.1` and native git
identity `9e02a51d7f0e4a231e2f71fe6d19ce02724277c9` before the response and
counters. Do not generate file-lane calls for the registry `1.4.6` wheel. Use
the exact `2.1.0` artifact or a later verified registry release for file-lane
code.

## Verification

Release intake checks the PyPI JSON version, filename, yanked state, and
SHA-256, then installs from `https://pypi.org/simple` in a clean environment.
Payload presence is not used as a substitute for matching-host execution.

For connector APIs and runnable Python code, use the
[`coakka-v2-connector` source README](https://github.com/phuong-tran/coakkaJVMConnector/tree/main/python)
and the [Python samples](https://github.com/phuong-tran/coakka-samples/tree/main/runtime/python).
For loader, architecture, certificate, and publisher-trust failures, use
[Troubleshooting](../../docs/troubleshooting.md).
