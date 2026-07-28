# NuGet Package Lane

NuGet is the public package-manager lane for C#/.NET CoAkka packages.

Current coordinates:

| Surface | Coordinate | Native generation |
| --- | --- | --- |
| Runtime | `CoAkka.Runtime`, exact `1.3.3` | `1.3.2+caff6d6d` |
| Logger | `CoAkka.Logger`, exact `1.2.2` | `1.2.1+f50756ebff0d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.3.3
dotnet add package CoAkka.Logger --version 1.2.2
```

The runtime and logger packages bundle native assets under
`runtimes/<rid>/native` for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`
