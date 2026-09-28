# Consuming The Go Archive

```sh
tar -xzf coakka-http-go-1.0.0.tar.gz
go mod edit -require=github.com/phuong-tran/coakka-http-runtime-go@v0.0.0
go mod edit -replace=github.com/phuong-tran/coakka-http-runtime-go=/absolute/path/coakka-http-go-1.0.0
go mod tidy
```

Import `github.com/phuong-tran/coakka-http-runtime-go`. No separately
installed native runtime is required. A C11 compiler is required when building
the cgo bridge; consumers do not build native runtime dependencies.

