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
| Runtime | `CoAkka.Runtime`, exact `1.4.6` | `1.4.0+2cee86bf` |
| Logger | `CoAkka.Logger`, exact `1.2.2` | `1.2.1+f50756ebff0d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.4.6
dotnet add package CoAkka.Logger --version 1.2.2
```

The runtime package bundles native assets under `runtimes/<rid>/native` for:

- `osx-arm64`
- `linux-arm64`
- `win-x64`

The logger package remains at `1.2.2` with its separately documented five-RID
native matrix.
