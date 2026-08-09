# NuGet C# Runtime 2.1.0

Published `CoAkka.Runtime==2.1.0` to NuGet on August 9, 2026.

## Artifact Identity

- Connector source: `99bb16ce878ad3b00c4c8bdc140972ca9f4ad28e`
- Native generation: `2.1.0+60ddf70d`
- Native source: `60ddf70d63b94750bb76c8284923e73199788c2e`
- Pushed candidate SHA-256:
  `7a0a5fbc26d26dd282be2bdc01adb38bafd54dc5c00acfb945e1ec7f0cb01c95`
- NuGet repository-signed download SHA-256:
  `d4a4628e95cd608a84a21c1a86b2441211e4f490e25674856d561bce9b960bc6`

The package contains exactly five native runtime libraries under standard RID
paths: `linux-arm64`, `linux-x64`, `osx-arm64`, `win-arm64`, and `win-x64`.

## Registry Verification

The staged candidate passed NuGet readiness and packaged request/reply plus
route-miss deadletter smoke before upload. The public registry download then
passed the same readiness and execution smoke on macOS ARM64.

`dotnet nuget verify --all` validates the NuGet.org repository signature.
NuGet.org added only `.signature.p7s`; every other candidate ZIP entry remains
present and byte-identical.

The package README links to the canonical public file-lane contract. Readiness
now rejects links to the non-public connector source repository.

Install:

```sh
dotnet add package CoAkka.Runtime --version 2.1.0
```
