# 2026-07-25 Go Module Release

The Go runtime and logger lanes are now public Go modules.

Current module coordinates:

- `github.com/phuong-tran/coakka-runtime-go@v1.3.3`
- `github.com/phuong-tran/coakka-logger-go@v1.2.2`

Current module repositories:

- `https://github.com/phuong-tran/coakka-runtime-go`, commit `60f217d`, tag `v1.3.3`
- `https://github.com/phuong-tran/coakka-logger-go`, commit `ac72cde`, tag `v1.2.2`

The `v1.3.3` and `v1.2.2` patch tags add package-level `doc.go` onboarding so
pkg.go.dev shows useful package documentation instead of only the API index.

Onboarding docs are present in the tagged module commits:

- `coakka-runtime-go@60f217d`
- `coakka-logger-go@ac72cde`

The tarball artifacts remain the GitHub artifact-mirror surface:

- `runtime/go/releases/1.3.2+caff6d6d-6d5ea58/coakka-v2-connector-go-1.3.2.tar.gz`
- `logger/go/releases/1.2.1+f50756ebff0d/coakka-logger-go-1.2.1.tar.gz`

Readiness gate:

```sh
# From the connector release workspace:
CONNECTOR_ROOT="$(pwd)"

cd "${CONNECTOR_ROOT}/go"
go test ./...
bash scripts/smoke-packaged-package.sh
bash scripts/export-module-repo.sh /tmp/coakka-runtime-go-module
cd /tmp/coakka-runtime-go-module
go test ./...

cd "${CONNECTOR_ROOT}/logger/go"
go test ./...
bash scripts/smoke-packaged-package.sh
bash scripts/export-module-repo.sh /tmp/coakka-logger-go-module
cd /tmp/coakka-logger-go-module
go test ./...
```

Clean consumer verification:

```sh
GOPROXY=https://proxy.golang.org,direct go get github.com/phuong-tran/coakka-runtime-go@v1.3.3
GOPROXY=https://proxy.golang.org,direct go get github.com/phuong-tran/coakka-logger-go@v1.2.2
GOPROXY=https://proxy.golang.org,direct go test github.com/phuong-tran/coakka-runtime-go
GOPROXY=https://proxy.golang.org,direct go test github.com/phuong-tran/coakka-logger-go
```

The default Go checksum proxy returned transient `500` responses while the new
public module tags were fresh, then passed after the proxy indexed the tags. No
local `replace` was used.
