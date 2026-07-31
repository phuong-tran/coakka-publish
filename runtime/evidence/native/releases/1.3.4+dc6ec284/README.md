# CoAkka Runtime Native Evidence Runner 1.3.4

This release contains prebuilt public-ABI evidence runners for CoAkka Runtime
native generation `1.3.4+dc6ec284`.

The runners are built from the public C source under:

```text
coakka-samples/runtime/evidence/native/
```

They do not contain or reach into private core runtime source.

## Platforms

- `linux-aarch64`
- `linux-x86_64`
- `macos-aarch64`
- `windows-aarch64`
- `windows-x86_64`

Each archive bundles one evidence executable and the matching published runtime
shared library.

## Evidence Scope

The runner supports `smoke`, `pressure`, `stress`, and `soak`. Every passing
run enforces target route, terminal-outcome, reply, deadletter, and bounded
admission invariants and emits one JSON document.

Prefer a controlled Linux host when measurements will inform Kubernetes or
container deployment decisions. Linux, macOS, and Windows VM runs prove
portability and correctness, but their throughput values must not be compared
with host or production results.

## Run

Use the wrapper from `coakka-samples`:

```sh
bash run.sh runtime/evidence/native smoke
```

On Windows PowerShell:

```powershell
./runtime/evidence/native/run.ps1 smoke
```
