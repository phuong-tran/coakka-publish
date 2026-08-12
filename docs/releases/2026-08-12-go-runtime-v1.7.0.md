# Go Runtime v1.7.0

Date: 2026-08-12

The public Go runtime coordinate is now:

```text
module: github.com/phuong-tran/coakka-runtime-go
version: v1.7.0
tag: v1.7.0
commit: 0901089ac37641e6722caf4b187bc0670b7a96e5
connector version: 2.4.0
native generation: 2.4.0+c2f53117
Go module sum: h1:jAbDc00DCd7eb1Hz0RwkFypAxiSnMrQb1bcqlISqCvI=
```

This release adds explicit embedded, outbound-only, and network-node startup
modes. Embedded and outbound-only runtimes do not allocate a runtime TCP
listener; local routes use port `0`. A network node requires explicit bind and
advertise endpoints and fails startup closed when bind or configuration apply
fails.

The tag bundles exact native payloads for macOS ARM64, Linux ARM64/x86-64, and
Windows ARM64/x86-64. All five payloads match the staged
`2.4.0+c2f53117-0afb5e9` candidate byte-for-byte and pass format and package
verification. Source tests, package smoke, and a clean Go-proxy request/reply
consumer pass on macOS ARM64. Payload presence is not matching-host Go
execution evidence for Linux or Windows.

Publisher signing remains absent. The release is an annotated Git tag consumed
by Go modules; no GitHub Release object is created.
