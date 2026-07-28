# NuGet Package Lane

NuGet is the public package-manager lane for C#/.NET CoAkka packages.

Current coordinates:

| Surface | Coordinate | Native generation |
| --- | --- | --- |
| Runtime | `CoAkka.Runtime`, exact `1.3.3` | `1.3.2+caff6d6d` |

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.3.3
```

The runtime package bundles native assets under `runtimes/<rid>/native` for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

The C# logger remains in the GitHub artifact mirror until a separate NuGet
logger release gate is opened.
