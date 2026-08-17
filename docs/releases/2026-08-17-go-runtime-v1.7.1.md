# Go Runtime v1.7.1

Date: 2026-08-17

The public Go runtime coordinate is:

```text
module: github.com/phuong-tran/coakka-runtime-go
version: v1.7.1
annotated tag object: de7dd7122cb92e6b6266a399935add46d21b614c
tag commit: 4866ce46162d4f0384c1957efbd6176e69a1925b
connector source checkpoint: 2c6039e835b192d2e9a1d63468ba588dae220686
connector version: 2.4.0
native generation: 2.4.0+c2f53117
Go compatibility floor: 1.22
Go module sum: h1:zE2Gfrc4VZO+g01RL3YHM/dqxZSX3Qjmfc9C7tK/fCU=
Go go.mod sum: h1:YBxjoy2dFSIW9iBvAcZk1NGWQ9yqxjAkjDWv47mJp9M=
```

This compatibility-only patch lowers the module `go` directive from `1.23.0`
to `1.22`, which is the minimum declared by the direct
`google.golang.org/protobuf v1.36.6` dependency. The JVM-Go demo removes an
unused `golang.org/x/sys v0.35.0` requirement that otherwise imposed Go 1.23.
Go 1.22 is a compatibility floor, not a production toolchain recommendation;
production builds should use a currently supported Go release.

Public Go CI run `32038021260` passes module tidiness, source tests, public
surface checks, and all five native-payload checks on Linux x86-64 with Go
`1.22.12` and stable Go `1.26.6`. Follow-up run `32038213283` passes the same
matrix for the final tag commit. Connector run `32038008116` passes the full
connector gates for the implementation checkpoint. On macOS ARM64, both Go
toolchains pass source tests, vet, packaged-consumer request/reply, live
integration, and clean remote-tag basic/deadletter samples. Stable Go also
passes the race detector and `govulncheck` reports no reachable vulnerability.
The Spring Boot-Go scenario compiles its Go store from the remote tag with Go
`1.22.12`.

The public API and production Go source are unchanged except for the package
installation example in `doc.go`. Every native tree object is byte-identical
to `v1.7.0`; the patch retains connector `2.4.0` and native generation
`2.4.0+c2f53117`.

## Systems Review

| Control | Result | Evidence and release impact |
| --- | --- | --- |
| Ownership, lifecycle, shutdown, borrowing, queues, waits, threads, and descriptors | Not applicable | No runtime or connector behavior changed. Existing contracts remain authoritative. |
| Senior systems review | Pass | Compatibility metadata, dependency graph, package projection, fail-closed module tidiness, and consumer execution were reviewed. No heap, cacheline, wakeup, fd, or transport path changed. |
| Expert systems review | Not applicable | No kernel boundary, memory ordering, MMU/DMA, IRQ, NUMA, driver, or hardware-timing mechanism changed. |
| Static and dependency analysis | Pass | `go vet` passes on stable Go; `govulncheck` reports no reachable vulnerability; affected shell scripts pass syntax checks and ShellCheck with only reviewed pre-existing source/trap dispositions excluded. |
| Internal and public tests | Pass | Runtime source, JVM-Go demo, public module, installed package, request/reply, deadletter, and scenario compilation pass at the compatibility floor and current stable Go. |
| Race, sanitizer, fuzz, pressure, fault, soak, and performance | Not applicable | Go race testing passes. Native sanitizer/fuzz/load controls are not rerun because no Go runtime implementation, C/C++ source, native ABI, payload, concurrency, parser, resource bound, or performance claim changed. |
| Artifact and platform evidence | Pass with stated limits | Go proxy origin, checksum database, annotated tag, five unchanged native payloads, macOS ARM64 execution, and Linux x86-64 dual-toolchain CI pass. Prior matching-host Linux native-generation evidence remains valid; no matching Go-on-Windows execution is claimed. |

Publisher signing remains absent. The GitHub Release contains no duplicate
binary assets because the Go module already embeds its five platform payloads.
