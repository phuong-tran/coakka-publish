# Consuming The Go Archive

Unpack the archive into an application-controlled dependency directory, then
use a local replacement until the public Go module tag is opened:

```sh
tar -xzf coakka-http-go-1.0.0.tar.gz
go mod edit -require=github.com/phuong-tran/coakka-http-runtime-go@v0.0.0
go mod edit -replace=github.com/phuong-tran/coakka-http-runtime-go=/absolute/path/coakka-http-go-1.0.0
go mod tidy
```

Import `github.com/phuong-tran/coakka-http-runtime-go`. The build-tag-selected
Core payload is extracted to an owner-only process directory, verified by
SHA-256 and retained for process lifetime. Applications do not install a
separate native runtime.

`Builder`/`Service` is the buffered request/reply convenience path. Use
`Config`/`Core` for streaming, trailers, SSE, WebSocket, outbound, rebind,
health, inspection and monitoring. Read the archived `README.md`,
`ARCHITECTURE.md`, and `KNOWN_LIMITATIONS.md` before using the low-level lanes.
