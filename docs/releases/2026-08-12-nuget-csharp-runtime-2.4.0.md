# NuGet C# Runtime 2.4.0

CoAkka published `CoAkka.Runtime==2.4.0` to NuGet on August 12, 2026.

## Artifact Identity

- Connector source: `0afb5e9a7c19d002ec4fd1cd2d1044e86b096d5e`
- Payload staging source: `ef40ada4df41ee3fc7c9d21c2f4c4cad9c95b582`
- Native generation: `2.4.0+c2f53117`
- Native source: `c2f53117f991f67f809a0bf46bac2ce26091eb78`
- Pushed candidate SHA-256:
  `d0888ceb902641b141aabec9d8d650f9912dbe92583c1de870bdb97b1862df20`
- NuGet repository-signed download SHA-256:
  `c00ecf62b691937f7ad05f8178ea45a383c9a1de093463e1914fb73a4210c182`

The package contains exactly five native runtime libraries under standard RID
paths: `linux-arm64`, `linux-x64`, `osx-arm64`, `win-arm64`, and `win-x64`.
It adds explicit embedded, outbound-only, and network-node participation modes
to the C# surface while retaining request/reply, deadletters, File Lane, and
Stream Lane.

## Registry Verification

The staged candidate passed NuGet readiness and packaged request/reply plus
route-miss deadletter smoke before upload. A clean NuGet.org install then
loaded runtime `2.4.0` generation `c2f53117` and repeated request/reply plus
matched deadletter on macOS ARM64. A live Spring Boot-to-C# two-process sample
then completed diagnostics, route miss, create, update, list, and delete over
explicit loopback `NETWORK_NODE` listeners.

`dotnet nuget verify --all` validates the NuGet.org repository signature.
NuGet.org added only `.signature.p7s`; all 16 candidate ZIP entries remain
present and byte-identical. The Linux and Windows RID assets retain their
release build and package-verification evidence; this receipt does not claim a
new post-publication execution on those hosts.

Install:

```sh
dotnet add package CoAkka.Runtime --version 2.4.0
```
