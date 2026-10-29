# C# Logger NuGet 1.2.2

`CoAkka.Logger` is published on NuGet for .NET consumers.

## Coordinate

```sh
dotnet add package CoAkka.Logger --version 1.2.2
```

Package URL:
https://www.nuget.org/packages/CoAkka.Logger/1.2.2

## Native Generation

- logger native generation: `1.2.1+f50756ebff0d`
- supported RIDs: `osx-arm64`, `linux-arm64`, `linux-x64`, `win-arm64`,
  `win-x64`

The package carries the C# API, public README, license file, release metadata,
and RID native assets under the NuGet `runtimes/<rid>/native` layout.

## Verification

- package readiness gate passed:
  `logger/csharp/scripts/check-nuget-readiness.sh`
- packaged consumer smoke passed from the local package output
- NuGet push accepted `CoAkka.Logger 1.2.2`
- clean consumer install from `https://api.nuget.org/v3/index.json` passed
- public NuGet smoke started a logger host, emitted one accepted record,
  drained it, and verified queue pressure counters

The C# logger NuGet lane is now current alongside the C# runtime NuGet lane.
