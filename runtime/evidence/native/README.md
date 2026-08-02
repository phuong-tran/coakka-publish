# Native Runtime Evidence Artifacts

Native evidence runners are built from the public sample source:

```text
coakka-samples/runtime-test/
```

The auditable C11 source is also included at the repository root under
[`runtime-test/`](../../../runtime-test/README.md). Its SHA-256 manifest and verification script let a
consumer inspect and verify the exact public-ABI harness without relying on a
second checkout.

They consume the published CoAkka Runtime native C ABI and bundle the matching
runtime native shared library for the target platform.

Current prebuilt platforms are:

- `linux-aarch64`
- `linux-x86_64`
- `macos-aarch64`
- `windows-aarch64`
- `windows-x86_64`

The sample directory separates process orchestration, CLI/config parsing,
runtime driving and ABI adaptation, and final JSON reporting. It contains no
private core runtime implementation.

The source also builds
`coakka_runtime_v2_connection_strategy_evidence`. That executable checks all
four connection modes against effective capability truth, validates structured
atomic rejection, proves invalid applies preserve active state, and verifies
that post-start reconfiguration fails without mutation. It explicitly does not
claim a TLS/mTLS certificate handshake; TLS diagnosis remains documented in
[Troubleshooting](../../../docs/troubleshooting.md).

The measured path is:

```text
caller request
  -> bounded admission
  -> route snapshot
  -> stable target
  -> local echo handler
  -> terminal reply
  -> caller drain
```

Use the source-first sample path when possible:

```sh
bash run.sh runtime-test smoke
```

Use the prebuilt path when the local machine does not have a C toolchain:

```sh
COAKKA_NATIVE_EVIDENCE_USE_PREBUILT=1 bash run.sh runtime-test smoke
```

Windows uses the prebuilt path directly:

```powershell
./runtime-test/run.ps1 smoke
```

Interpret the JSON output as a native public-ABI baseline on the current
machine. Connector, framework, network, TLS, business serialization, logging,
and production fleet costs are outside this lane. Prefer a controlled Linux
host for deployment-oriented measurements. Treat Docker, CI, UTM, and other VM
results as correctness and portability evidence, not throughput comparisons.

Verify the included source projection with:

```sh
bash scripts/verify-native-evidence-source.sh
```
