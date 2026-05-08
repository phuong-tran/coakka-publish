# Consuming The Go Logger Package

The Go logger package is a source package plus embedded native logger libraries.
Extract the archive and use a local `replace` until a public module path is
chosen.

Example consumer `go.mod`:

```go
module my-logger-consumer

go 1.22

require github.com/phuong-tran/coakka-logger-go v0.0.0

replace github.com/phuong-tran/coakka-logger-go => ./coakka-logger-go-0.1.0
```

Example:

```go
package main

import logger "github.com/phuong-tran/coakka-logger-go"

func main() {
	log, err := logger.Start(logger.LoggerSpec{
		SystemName: "sample",
		MinLevel:   logger.LevelInfo,
	}, "")
	if err != nil {
		panic(err)
	}
	defer log.Close()

	_, _, _ = log.Info("sample", "hello from Go")
}
```

Library resolution order:

- explicit path passed to `Start(spec, loggerLibPath)` or `ReadInfo(loggerLibPath)`
- `$COAKKA_LOGGER_LIB`
- packaged native library under `native/<platform>/`
- local fallback candidates under `lib/` and `logger/go/lib/`

Current packaged platforms:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
