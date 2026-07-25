# 2026-07-25 Go Module Readiness

The Go runtime and logger lanes are ready to become public Go modules after
their dedicated module repositories are opened and tagged.

Current module paths:

- `github.com/phuong-tran/coakka-runtime-go`
- `github.com/phuong-tran/coakka-logger-go`

Current tarball artifacts remain the public consumable surface:

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

Release blocker before documenting `go get`:

- create public repository `phuong-tran/coakka-runtime-go`
- create public repository `phuong-tran/coakka-logger-go`
- push the exported module roots
- tag `coakka-runtime-go` as `v1.3.2`
- tag `coakka-logger-go` as `v1.2.1`
- verify clean consumer installs without local `replace`
