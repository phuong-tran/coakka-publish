# Runtime Go Release Checklist

Package identity:

- Go package: `2.5.2`
- public Go module tag: `v1.8.2`
- native runtime core: `2.5.0`
- connector source checkpoint: `2c6039e`
- native generation: `2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a`

Publisher signing is absent. Platform execution evidence is recorded
separately from source compilation and bundled native payloads.

Release checklist:

```sh
cd go
bash scripts/stage-runtime-natives.sh
GOTOOLCHAIN=go1.22.12 go test ./...
GOTOOLCHAIN=go1.26.6 go test ./...
bash scripts/smoke-packaged-package.sh
bash scripts/verify-native-payload.sh
```

For host-specific consumer verification:

```sh
COAKKA_V2_HOST_RUNTIME_LIB=/abs/path/to/libcoakka_runtime_v2.dylib \
  bash scripts/smoke-packaged-package.sh
```

Host-only output is evidence for that host only, not multi-platform execution
evidence.

Public Go module export checklist:

```sh
cd go
bash scripts/export-module-repo.sh /tmp/coakka-runtime-go-module
cd /tmp/coakka-runtime-go-module
GOTOOLCHAIN=go1.22.12 go test ./...
GOTOOLCHAIN=go1.26.6 go test ./...
```

The exported module is suitable for clean-consumer verification. Bundled
payload presence and cross-compilation do not claim Linux/Windows execution.
Contact `gabrielgun1983@gmail.com` and see
[common troubleshooting](https://github.com/phuong-tran/coakka-samples/blob/main/docs/troubleshooting.md).
