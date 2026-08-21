# PyPI Package Lane

PyPI is the public package-manager lane for the Python CoAkka runtime and
logger connectors. Both packages load an architecture-matched native library;
their Python package version and bundled native generation are separate
identities.

## Current Coordinates

| Surface | Install | Native generation |
| --- | --- | --- |
| Runtime | `python -m pip install coakka-v2-connector==2.5.3` | `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be` |
| Logger | `python -m pip install coakka-logger==1.2.2` | `1.2.1+f50756ebff0d` |

The machine-readable release receipt is [`current.json`](current.json). Pin the
versions above; the current coordinate does not depend on deleting historical
registry files.

## Runtime 2.5.3

PyPI and the artifact mirror contain `coakka_v2_connector-2.5.3-py3-none-any.whl`
from connector source `0ba485e` over native generation
`2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`. The
wheel contains Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64
libraries and exposes both File Lane and Stream Lane. Package, digest,
metadata, request/reply, File Lane, and Stream Lane gates pass on the exact
candidate. PyPI JSON reports SHA-256
`18f87998c33ba6e5bf43e0981f39f97e229e2dc74d44b3f762698ac46d6deff5`,
and the downloaded registry wheel is byte-identical to the candidate.
See the [Runtime registry packages 2.5.3 receipt](../../docs/releases/2026-08-21-runtime-registries-2.5.3.md)
for the complete registry comparison.

## Runtime Platforms And Evidence

The runtime wheel contains native payloads for Linux ARM64/x86-64, macOS
ARM64, and Windows ARM64/x86-64. All five payloads pass package, binary-format,
architecture, and digest verification.

Runtime `2.5.3` exposes typed File receive and Stream publish owner grants,
validated trusted reconstruction, and exact replica-owner endpoint pinning.

Exact PyPI `2.5.3` request/reply executes from a clean registry install on
macOS ARM64. The byte-identical candidate also passes Python File Lane and
Stream Lane tests there. Cross-platform execution claims remain limited to the
evidence recorded for each host.

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

The sample prints connector-facing runtime version `2.5.1` and native git
identity `26f7944de4a4e0598845a54e4775f9463a9e33be` before the response and
counters. The registry `2.5.3` wheel exposes File Lane and Stream Lane.

## Verification

Release intake checks the PyPI JSON version, filename, yanked state, and
SHA-256, then installs from `https://pypi.org/simple` in a clean environment.
Payload presence is not used as a substitute for matching-host execution.

For connector APIs and runnable Python code, use the public
[`coakka-v2-connector` artifact README](../../runtime/python/releases/2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be-0ba485e/README.md)
and the [Python samples](https://github.com/phuong-tran/coakka-samples/tree/main/runtime/python).
For loader, architecture, certificate, and publisher-trust failures, use
[Troubleshooting](../../docs/troubleshooting.md).
