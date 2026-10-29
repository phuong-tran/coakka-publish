# C# Runtime NuGet 1.3.3

This note records the first public NuGet registry release for the C# runtime
connector.

| Surface | Coordinate | Native generation |
| --- | --- | --- |
| C# runtime | `CoAkka.Runtime==1.3.3` | `1.3.2+caff6d6d` |

The NuGet package bundles the runtime native assets under the standard
`runtimes/<rid>/native` layout for:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

Registry package:

```text
https://www.nuget.org/packages/CoAkka.Runtime/1.3.3
```

Clean install:

```sh
dotnet add package CoAkka.Runtime --version 1.3.3
```

Verification:

- C# NuGet readiness gate passed in `coakkaJVMConnector`.
- Packaged consumer smoke passed from the generated package.
- NuGet push succeeded for `CoAkka.Runtime.1.3.3.nupkg`.
- Clean consumer restore from `https://api.nuget.org/v3/index.json` passed.
- Clean consumer runtime host smoke started the bundled native runtime and
  completed a request/reply path.

The GitHub artifact mirror for the C# runtime lane remains available under
`runtime/csharp/releases/`. The NuGet coordinate above is now the normal
package-manager install path for C# runtime consumers.
