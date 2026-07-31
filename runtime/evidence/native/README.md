# Native Runtime Evidence Artifacts

Native evidence runners are built from the public sample source:

```text
coakka-samples/runtime/evidence/native/
```

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
bash run.sh runtime/evidence/native smoke
```

Use the prebuilt path when the local machine does not have a C toolchain:

```sh
COAKKA_NATIVE_EVIDENCE_USE_PREBUILT=1 bash run.sh runtime/evidence/native smoke
```

Windows uses the prebuilt path directly:

```powershell
./runtime/evidence/native/run.ps1 smoke
```

Interpret the JSON output as a native public-ABI baseline on the current
machine. Connector, framework, network, TLS, business serialization, logging,
and production fleet costs are outside this lane. Prefer a controlled Linux
host for deployment-oriented measurements. Treat Docker, CI, UTM, and other VM
results as correctness and portability evidence, not throughput comparisons.
