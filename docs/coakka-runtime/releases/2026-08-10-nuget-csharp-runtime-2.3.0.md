# NuGet C# Runtime 2.3.0

Published `CoAkka.Runtime==2.3.0` to NuGet on August 10, 2026.

## Artifact Identity

- Connector source: `3a84c7b658e97d4aa989283103cc561c89420685`
- Payload staging source: `7708105b41cbe121bd76432c5fde66905ecbb0c0`
- Native generation: `2.3.0+a83ab412`
- Native source: `a83ab41207821388eb5507be63d81ba721d6d585`
- Pushed candidate SHA-256:
  `49317de074f399a1f6a45a6c310844d900f34d3d20071774e1d11039fcc1159d`
- NuGet repository-signed download SHA-256:
  `93f21f899b069332426a87fb7ca30f94f4417853dc79c5daca398270ae983879`

The package contains exactly five native runtime libraries under standard RID
paths: `linux-arm64`, `linux-x64`, `osx-arm64`, `win-arm64`, and `win-x64`.

## Registry Verification

The staged candidate passed NuGet readiness and packaged request/reply plus
route-miss deadletter smoke before upload. File Lane transferred `9,437,915`
bytes and Stream Lane transferred `33` ordered frames / `2,097,883` bytes on
macOS ARM64. A clean NuGet.org install then loaded runtime `2.3.0` generation
`a83ab412` and passed request/reply plus matched deadletter.

`dotnet nuget verify --all` validates the NuGet.org repository signature.
NuGet.org added only `.signature.p7s`; all 16 candidate ZIP entries remain
present and byte-identical.

Install:

```sh
dotnet add package CoAkka.Runtime --version 2.3.0
```
