# Consuming The Go Runtime Package

The Go runtime package is a source package plus embedded native runtime
libraries. Extract the archive and use a local `replace` until a public module
path is chosen.

Example consumer `go.mod`:

```go
module my-runtime-consumer

go 1.23.0

require github.com/phuong-tran/coakka-runtime-go v0.0.0

replace github.com/phuong-tran/coakka-runtime-go => ./coakka-v2-connector-go-0.2.0
```

Example:

```go
package main

import (
	"time"

	connector "github.com/phuong-tran/coakka-runtime-go"
)

func main() {
	runtimeHost, err := connector.StartRuntimeHost(connector.ConnectorStartSpec{
		SystemName: "sample",
		NodeID:     "sample-node",
		Routes: []connector.RouteSpec{{
			Target: "svc.echo",
			Endpoints: []connector.EndpointSpec{{
				Host:  "127.0.0.1",
				Port:  19291,
				Flags: uint32(connector.EndpointFlagLocal),
			}},
		}},
	}, "")
	if err != nil {
		panic(err)
	}
	defer runtimeHost.Close()

	_, _ = runtimeHost.AwaitNextMonitor(10 * time.Millisecond)
}
```

Library resolution order:

- explicit path passed to `StartRuntimeHost(startSpec, runtimeLibPath)`
- `$COAKKA_RUNTIME_LIB`
- packaged native library under `native/<platform>/`
- local fallback candidates under `lib/`

Current packaged platforms:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`

One Go process may start one active runtime host. `StartConnectorOrchestrator`
remains as the compatibility name for the same lifecycle owner.
