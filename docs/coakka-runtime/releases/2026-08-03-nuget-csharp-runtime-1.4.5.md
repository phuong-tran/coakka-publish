# NuGet C# Runtime 1.4.5

Published `CoAkka.Runtime==1.4.5` to NuGet on August 3, 2026.

## Artifact Identity

- NuGet package version: `1.4.5`
- Connector source: `562028dc52de6fc2497f9f17748945d8e917563e`
- Native generation: `1.4.0+2cee86bf`
- Native source: `2cee86bf89f38725aa763bb3abd96b64a904875d`
- Pushed candidate SHA-256:
  `bead296453e17304f90ecdc04aaabbd3f81d2f99c6c6bae5bb3636ad3651c191`
- NuGet repository-signed download SHA-256:
  `b8772cefb53dd87170f8d62cdd83f328da473e46a22be519de73d58904b42d3b`

The package version advances independently from the native runtime version.
No native binary, public ABI, native checksum, capability, or publisher-signing
state changed. The package embeds the same verified native bytes distributed
in the public runtime 1.4.0 artifact train for:

- `osx-arm64`
- `linux-arm64`
- `win-x64`

## Registry Signature

NuGet adds a valid Microsoft NuGet.org repository signature after upload. The
repository-signed archive contains one additional entry, `.signature.p7s`.
Every package entry from the pushed candidate, including all three native
libraries, remains byte-identical. This repository signature is registry
provenance; the native binaries remain unsigned by the publisher and runtime
behavior is unchanged.

## Verification

The candidate passed the C# NuGet readiness gate, exact three-RID package
audit, public runtime intake gate, transport smoke, and packaged request/reply
plus matched-deadletter smoke. Connector hosted CI run `30765335526` passed.

After publication, a clean .NET 10 project restored the exact package from
nuget.org, loaded runtime version `1.4.0` from source `2cee86bf`, and completed
request/reply plus matched route-miss deadletter handling. The downloaded
package also passed `dotnet nuget verify --all`.

Install:

```sh
dotnet add package CoAkka.Runtime --version 1.4.5
```

CoAkka Public Artifact License 1.1 applies. Publisher signing remains absent
and is not a runtime or package gate.
