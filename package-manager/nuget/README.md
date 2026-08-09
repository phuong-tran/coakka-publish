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
| Runtime | `CoAkka.Runtime`, exact `1.4.7` | `1.4.1+9e02a51d` |
| Logger | `CoAkka.Logger`, exact `1.2.2` | `1.2.1+f50756ebff0d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.4.7
dotnet add package CoAkka.Logger --version 1.2.2
```

The artifact mirror also contains `CoAkka.Runtime.2.1.0.nupkg` from connector
source `4782dcd` over native generation `2.1.0+60ddf70d`. Its five RID assets,
package metadata, checksums, and packaged request/reply/deadletter smoke pass.
This file is not presented as a NuGet.org coordinate until registry upload and
clean-feed verification complete.

The runtime package bundles native assets under `runtimes/<rid>/native` for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

The exact registry runtime package executes request/reply and route-miss
deadletter on macOS ARM64, Linux ARM64/x86-64, and Windows 11 ARM64. Windows
x86-64 retains package/RID verification without a matching-host connector run.
See [package and platform evidence](../../docs/runtime-package-platform-evidence.md).

The logger package remains at `1.2.2` with its separately documented five-RID
native matrix.
