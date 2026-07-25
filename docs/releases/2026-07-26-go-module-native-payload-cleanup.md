# Go Module Native Payload Cleanup

This note records the Go module patch release that removes duplicate
unversioned native-library aliases from the public Go module repositories and
adds package license metadata for pkg.go.dev.

Current module coordinates:

- `github.com/phuong-tran/coakka-runtime-go@v1.3.5`
- `github.com/phuong-tran/coakka-logger-go@v1.2.4`

Native generations are unchanged:

- runtime: `1.3.2+caff6d6d`
- logger: `1.2.1+f50756ebff0d`

The public module payload now carries one native library per supported platform,
using the generation-pinned filename only:

- macOS ARM64
- Linux ARM64
- Linux x86_64
- Windows ARM64
- Windows x86_64

Source commits:

- `coakkaJVMConnector@e2ea901`
- `coakka-runtime-go@36a3566`, tag `v1.3.5`
- `coakka-logger-go@3fe8bd9`, tag `v1.2.4`

Package metadata:

- root `LICENSE` covers the Go connector source as Apache-2.0
- `NATIVE-LICENSE.md` carries the preview terms for bundled native libraries

Verification:

```sh
cd go
go test ./...
bash scripts/smoke-packaged-package.sh
bash scripts/verify-native-payload.sh

cd ../logger/go
go test ./...
bash scripts/smoke-packaged-package.sh
bash scripts/verify-native-payload.sh
```
