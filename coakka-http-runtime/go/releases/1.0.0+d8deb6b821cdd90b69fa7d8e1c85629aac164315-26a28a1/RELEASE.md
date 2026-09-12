# Go Release Evidence

- Source: `coakka-http-runtime-connector@26a28a1c4af3eb65334993532b4eaa082fc75438`.
- `go test ./...`, `go vet ./...`, `go test -race ./...` and twenty normal
  repetitions pass on macOS arm64.
- Exact embedded payload tests pass on Linux arm64/x86-64 and Windows
  arm64/x86-64. Windows test executables contain the same source and payloads.
- Fifteen public tests pass on both Windows process architectures.
- GoDoc coverage, payload digest, public ABI completeness, concurrency,
  terminal-after-stop and idempotent close checks pass.

The Raspberry Pi Go runtime does not support the race detector; the same ARM64
race suite passes on the UTM ARM64 host. Windows x86-64 uses OS emulation and is
not performance evidence.
