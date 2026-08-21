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
| Runtime | `CoAkka.Runtime`, exact `2.5.3` | `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be` |
| Logger | `CoAkka.Logger`, exact `1.2.3` | `1.2.1+f50756ebff0d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 2.5.3
dotnet add package CoAkka.Logger --version 1.2.3
```

Runtime comes from connector source
`0ba485e8ff19f3ce23902345cb445a1f652fe3f3`; Logger comes from
`801a0a6e67152465568c72246b112beb299360a3`. Runtime `2.5.3` embeds native
generation `2.5.1+26f7944de4a4e0598845a54e4775f9463a9e33be`; Logger `1.2.3`
retains `1.2.1+f50756ebff0d`. Both contain one `lib/net8.0` managed asset plus native
assets for the five RIDs below. `net8.0` is the minimum supported application
target, and the exact candidates execute on .NET 8, 9, and 10.

Runtime `2.5.3` exposes typed File receive and Stream publish owner grants,
validated trusted reconstruction, and exact replica-owner endpoint pinning.

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
the [Runtime registry packages 2.5.3 receipt](../../docs/releases/2026-08-21-runtime-registries-2.5.3.md),
the machine-readable [`current.json`](current.json) release receipt,
[runnable C# samples](https://github.com/phuong-tran/coakka-samples/tree/main/runtime/csharp),
and the [public artifact catalog](https://github.com/phuong-tran/coakka-publish).
