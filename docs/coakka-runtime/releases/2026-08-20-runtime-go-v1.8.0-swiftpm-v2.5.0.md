# Runtime Go v1.8.0 And SwiftPM v2.5.0

Date: 2026-08-20

This scoped release publishes the source-package coordinates below without
advancing Maven Central, npm, PyPI, NuGet, or their 2.4.x runtime packages:

```text
Go module: github.com/phuong-tran/coakka-runtime-go@v1.8.0
Go annotated tag object: db4945e773afd637df94925a88af49db4cce0aa6
Go tag commit: 962c75f37e65bfcb7078e8deee955922cec6f39b
Go module sum: h1:rXaYtIHMUjUoHEbK0FN7jnOGTNJoUuLwn3wRi1LZ4m4=
Go go.mod sum: h1:YBxjoy2dFSIW9iBvAcZk1NGWQ9yqxjAkjDWv47mJp9M=

SwiftPM repository: https://github.com/phuong-tran/coakka-runtime-swift.git
SwiftPM exact version: 2.5.0
Swift annotated tag object: 92427e266a1c3bc5cb7f5332d4aefaa5c65177ee
Swift tag commit: 27d2d8ee711017d62ef20a092c78bc8f32345c82

Connector release source: 6b56a27b2139c8abb26483a04c2e26b14c4ab2fd
Native generation: 2.5.0+4b65d0b2256037bf7fc180bfa6df8c41efc1dd6a
Internal Core tag: coakka-runtime-core-v2.5.0-4b65d0b2
```

Both packages expose File Lane and Stream Lane and contain the same five
native targets: Linux ARM64/x86-64, macOS ARM64, and Windows ARM64/x86-64.
Connector source material is Apache-2.0. Bundled CoAkka native material remains
under the CoAkka Public Artifact License 1.1; `PACKAGE-LICENSE.md` maps those
terms by file scope and `NOTICE` summarizes the aggregate.

The exported Go module passes source tests, required-native tests, `go vet`,
five-payload architecture inspection, Go proxy resolution, checksum-database
verification, and clean remote-tag basic/deadletter consumers on macOS ARM64.
The Swift package passes build, tests, package readiness, exact-payload checks,
File Lane and Stream Lane smokes, and a clean remote-tag request/reply consumer
on macOS ARM64. Linux and Windows package payloads retain the matching-host
native evidence recorded for the sealed Runtime 2.5.0 candidate; no matching
Swift or Go app-host execution is newly claimed for those platforms.

Publisher signing remains absent. GitHub Releases carry no duplicate binary
assets because each source package already embeds its native payloads.
