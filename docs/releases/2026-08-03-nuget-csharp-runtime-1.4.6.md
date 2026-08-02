# NuGet C# Runtime 1.4.6

Published `CoAkka.Runtime==1.4.6` to NuGet on August 3, 2026.

## Artifact Identity

- NuGet package version: `1.4.6`
- Connector source: `693f122c5bc65d8e61ba56997325030310e7ca83`
- Native generation: `1.4.0+2cee86bf`
- Native source: `2cee86bf89f38725aa763bb3abd96b64a904875d`
- Pushed candidate SHA-256:
  `04bda5dd20f3398bf299ddc128c0fe4a2dc399d93dd75066ac40897ad827545d`
- NuGet repository-signed download SHA-256:
  `d0764b3ac22161dfb19e7a6befef18f2fc49dbd7aaf9eab09fdd1c752ab644fb`

This is a gallery-only metadata patch. The package README now uses CommonMark
image syntax for the public CoAkka logo, and the C# transport API link points
to a public absolute URL. No runtime behavior, native binary, public ABI,
native checksum, capability, or publisher-signing state changed.

The package embeds the same verified native bytes distributed in the public
runtime 1.4.0 artifact train for:

- `osx-arm64`
- `linux-arm64`
- `win-x64`

## Registry Signature

NuGet adds a valid Microsoft NuGet.org repository signature after upload. The
repository-signed archive contains one additional entry, `.signature.p7s`.
Every package entry from the pushed candidate, including its README and all
three native libraries, remains byte-identical. This repository signature is
registry provenance; the native binaries remain unsigned by the publisher and
runtime behavior is unchanged.

## Verification

The candidate passed the C# NuGet readiness gate, exact three-RID package
audit, public runtime intake gate, transport smoke, and packaged request/reply
plus matched-deadletter smoke. Connector hosted CI run `30766310048` passed.

After publication, the registry download passed `dotnet nuget verify --all`.
Its only extra ZIP entry is `.signature.p7s`; no candidate entry was removed
or changed. The packaged README contains no HTML image tags or relative
Markdown document links.

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.4.6
```

CoAkka Public Artifact License 1.1 applies. Publisher signing remains absent
and is not a runtime or package gate.
