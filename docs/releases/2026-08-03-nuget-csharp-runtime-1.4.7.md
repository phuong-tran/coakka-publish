# NuGet C# Runtime 1.4.7

Published `CoAkka.Runtime==1.4.7` to NuGet on August 3, 2026.

## Artifact Identity

- Connector source: `af244b1e`
- Native generation: `1.4.1+9e02a51d`
- Native source: `9e02a51d7f0e4a231e2f71fe6d19ce02724277c9`
- Pushed candidate SHA-256:
  `b0d0c6ec7d2182151f64695832d63a1c61b5c3375fd763a3c5cb45c8110aba51`
- NuGet repository-signed download SHA-256:
  `05f8472a92d793821d3c18f37120b89721d0ed5650d54b7574e7c6d103a0f4e0`

The package contains native runtime libraries for `linux-arm64`, `linux-x64`,
`osx-arm64`, `win-arm64`, and `win-x64` under standard NuGet RID paths.

## Registry Verification

The exact staged candidate passed the NuGet readiness gate, public runtime
intake gate, and packaged request/reply plus matched-deadletter smoke. Connector
hosted CI run `30782900448` passed before upload.

After publication, the package was downloaded from NuGet.org and passed
`dotnet nuget verify --all`, readiness, and the packaged consumer execution
smoke. It loaded runtime `1.4.1` from the full native source commit. NuGet.org
added only `.signature.p7s`; every candidate ZIP entry remained present and
byte-identical.

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.4.7
```

CoAkka Public Artifact License 1.1 applies. Publisher signing remains absent;
the downloaded package carries the NuGet.org repository signature.
