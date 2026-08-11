# Releasing CoAkka.Runtime To NuGet

This lane publishes the C# runtime connector package `CoAkka.Runtime`.

Current candidate:

- package: `CoAkka.Runtime`
- staged version: `2.4.0`
- bundled native generation: `2.4.0+c2f53117`
- publisher signing: absent

## Gates

Run the local gates before publishing:

```sh
bash csharp/scripts/check-nuget-readiness.sh
bash csharp/scripts/smoke-packaged-package.sh
```

The package must contain only these native runtime assets:

- `runtimes/linux-arm64/native/libcoakka_runtime_v2.so`
- `runtimes/linux-x64/native/libcoakka_runtime_v2.so`
- `runtimes/osx-arm64/native/libcoakka_runtime_v2.dylib`
- `runtimes/win-arm64/native/libcoakka_runtime_v2.dll`
- `runtimes/win-x64/native/libcoakka_runtime_v2.dll`

All five assets must match the candidate ledger. This package-shape gate does
not replace Linux/Windows connector and consumer execution.

## Publish

Create a NuGet API key with `Push` scope, then publish through the guarded
script:

```sh
export NUGET_API_KEY='...'
export COAKKA_NUGET_PUBLISH_CONFIRM='CoAkka.Runtime/<approved-version>'
bash csharp/scripts/publish-nuget.sh
```

The source version is not a registry coordinate. The script checks that the
requested version is not already present on nuget.org, builds one package
candidate, runs readiness and packaged-consumer checks against those exact
bytes, verifies that the checksum did not change, and then pushes the same file
to `https://api.nuget.org/v3/index.json`.

## Post-Publish

After NuGet accepts and indexes the package:

```sh
tmp_dir="$(mktemp -d)"
dotnet new console -o "${tmp_dir}/consumer" --framework net10.0
dotnet add "${tmp_dir}/consumer/consumer.csproj" package CoAkka.Runtime --version <approved-version>
```

Then update the public release ledger from `coakkaCoreNativeDev` and sync it to
`coakka-publish`:

- NuGet package-manager lane
- compatibility matrix
- package-manager availability
- release note for the approved `CoAkka.Runtime` version
- README public status
