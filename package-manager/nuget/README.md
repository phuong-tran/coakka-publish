# NuGet Package Lane

NuGet is the public package-manager lane for C#/.NET CoAkka packages.

`CoAkka.Runtime` is the .NET connector in the polyglot, multi-language,
multi-platform CoAkka Runtime ecosystem. CoAkka is not a .NET-only runtime: the
package uses the same native core and public runtime contract as the JVM,
JavaScript, Python, Go, Rust, Swift, and other connector lanes. Kubernetes is
supported but not required.

Current coordinates:

| Surface | Coordinate | Native generation |
| --- | --- | --- |
| Runtime | `CoAkka.Runtime`, exact `2.3.0` | `2.3.0+a83ab412` |
| Logger | `CoAkka.Logger`, exact `1.2.2` | `1.2.1+f50756ebff0d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 2.3.0
dotnet add package CoAkka.Logger --version 1.2.2
```

The registry and artifact mirror contain `CoAkka.Runtime.2.3.0.nupkg` from
connector source `3a84c7b` over native generation `2.3.0+a83ab412`. Its five
RID assets, package metadata, checksums, packaged request/reply/deadletter,
File Lane, and Stream Lane smokes pass. NuGet.org adds only its repository
signature; every other ZIP entry is byte-identical to the staged candidate.

The runtime package bundles native assets under `runtimes/<rid>/native` for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

The exact registry runtime package executes request/reply and route-miss
deadletter on macOS ARM64. The repository-signed package preserves every
candidate entry, so the five-platform payload and candidate verification remain
applicable without claiming a new post-publication connector run on every host.
See [package and platform evidence](../../docs/runtime-package-platform-evidence.md).

The logger package remains at `1.2.2` with its separately documented five-RID
native matrix.
