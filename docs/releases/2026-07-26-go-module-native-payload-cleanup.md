# Go Module Native Payload Cleanup

This note records the Go module patch release that removes duplicate
unversioned native-library aliases from the public Go module repositories and
adds package license metadata for pkg.go.dev.

Current module coordinates:

- `github.com/phuong-tran/coakka-runtime-go@v1.3.6`
- `github.com/phuong-tran/coakka-logger-go@v1.2.5`

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

- `coakkaJVMConnector@b5aa219`
- `coakka-runtime-go@abec546`, tag `v1.3.6`
- `coakka-logger-go@62f9dee`, tag `v1.2.5`

Package metadata:

- root `LICENSE` covers the Go connector source as Apache-2.0
- `NATIVE-LICENSE.md` carries the preview terms for bundled native libraries
- `.github/FUNDING.yml` matches the public samples funding metadata
- README badges cover CI, pkg.go.dev, package version, GitHub release, license,
  and funding
- module CI runs `go test ./...` and checks the native payload shape

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
