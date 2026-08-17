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
| Runtime | `CoAkka.Runtime`, exact `2.4.1` | `2.4.0+c2f53117` |
| Logger | `CoAkka.Logger`, exact `1.2.3` | `1.2.1+f50756ebff0d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 2.4.1
dotnet add package CoAkka.Logger --version 1.2.3
```

The registry packages come from connector source
`801a0a6e67152465568c72246b112beb299360a3`. Runtime `2.4.1` retains native
generation `2.4.0+c2f53117`; Logger `1.2.3` retains native generation
`1.2.1+f50756ebff0d`. Both contain one `lib/net8.0` managed asset plus native
assets for the five RIDs below. `net8.0` is the minimum supported application
target, and the exact candidates execute on .NET 8, 9, and 10.

The runtime package bundles native assets under `runtimes/<rid>/native` for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

The exact repository-signed Runtime package executes request/reply and
route-miss deadletter on .NET 8, 9, and 10. The exact Logger package executes
accepted write, drain, and bounded-pressure drop behavior on the same host
matrix. NuGet.org adds only `.signature.p7s`; every candidate ZIP entry remains
byte-identical in each public package. Both repository signatures pass
`dotnet nuget verify --all`.

See [package and platform evidence](../../docs/runtime-package-platform-evidence.md),
the [NuGet release receipt](../../docs/releases/2026-08-17-dotnet-nuget-app-host-baseline.md),
[runnable C# samples](https://github.com/phuong-tran/coakka-samples/tree/main/runtime/csharp),
and the [public artifact catalog](https://github.com/phuong-tran/coakka-publish).
