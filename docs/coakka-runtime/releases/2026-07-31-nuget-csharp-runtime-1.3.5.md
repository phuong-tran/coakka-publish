# NuGet C# Runtime 1.3.5

Published `CoAkka.Runtime==1.3.5` to NuGet on July 31, 2026.

## Runtime Generation

The package embeds public native runtime generation:

```text
1.3.4+dc6ec284
```

Packaged RID coverage:

- `osx-arm64`
- `linux-arm64`
- `linux-x64`
- `win-arm64`
- `win-x64`

## Verification

The published coordinate was restored from nuget.org into a clean temporary
.NET 10 console project. The consumer smoke loaded the RID native asset,
completed a local request/reply round trip, and matched a route-miss
deadletter:

```text
CoAkka C# runtime smoke ok
runtime=1.3.4 git=dc6ec284 backend=tcp
response=echo-hello-csharp-runtime delivered=1 matched=1 deadletters=1
```

The package also passed its pre-publish readiness gate, package-content audit,
five-RID native coverage check, and private-path marker scan.

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.3.5
```
