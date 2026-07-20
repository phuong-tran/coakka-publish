# 2026-07-18 Public Release Boundary 1.3.1

This note records the public release boundary for the current CoAkka Runtime
`1.3.1` train. It is the provenance checkpoint for the artifact repository,
the runtime source repository, the runtime connector repository, and the public
sample repository.

## Public Artifact Boundary

| Surface | Public generation | Source or package snapshot |
| --- | --- | --- |
| Runtime native core | `1.3.1+bda2ef5` | `coakkaCoreNativeDev` source snapshot `bda2ef5` |
| Runtime JVM connector | `1.3.1-gbda2ef5-0a0aa76` | connector source snapshot `0a0aa76`, native core `bda2ef5` |
| Runtime non-JVM connectors | `1.3.1+bda2ef5-0a0aa76` | connector source snapshot `0a0aa76`, native core `bda2ef5` |
| Spring Boot and Quarkus adapters | `1.3.1-g0a0aa76` | connector source snapshot `0a0aa76` |
| CoAkka Runtime Client | `1.3.1+2215b0f` | `coakkaCoreNativeDev` client source snapshot `2215b0f` |
| CoAkka Runtime Inspect native UI | `1.3.1+d7ab7fa` | `coakkaCoreNativeDev` inspect source snapshot `d7ab7fa` |
| Public samples verified boundary | `450ed5d` | `coakka-samples` commit `450ed5d` |
| Public samples onboarding checkpoint | `85b2a8c` | docs and runner alias checkpoint over the same published artifacts |
| Public samples release-link checkpoint | `53ea3a6` | docs point direct downloads at the attached GitHub Release assets |
| Public samples walkthrough checkpoint | `a614565` | docs embed the runtime-client animated CLI walkthrough GIF |
| Public samples onboarding polish | `d89701e` | docs clarify `coakka-client` command naming, public runtime-client docs link, and Docker bundle output shape |
| Public manifest provenance checkpoint | `25179d6` manifest checksum prefix | manifest comments make Mojo/Zig source-package source commits explicit without changing artifact bytes |
| Public runtime-client Docker Hub demo image | `ab5c612` | sample image checkpoint for the prebuilt `coakka-client` walkthrough image |
| Public runtime-inspect Docker Hub sample image | `bc87a35` | sample image checkpoint for the prebuilt browser inspect walkthrough image |

The public artifact manifest checksum for this boundary is:

```text
e6c7122ba4ab755bc8287c8f9555faced97ec3645138754650e5e352e9115d79  artifacts/public-artifacts.tsv
```

The GitHub Release page for `coakka-public-artifacts-v1.3.1` attaches all 32
public artifact files listed by `artifacts/public-artifacts.tsv`, plus the
manifest itself, so users can download from either the release page or the
repository paths while verifying the same manifest SHA256 values.

## Repository Boundary

| Repository | Commit | Role |
| --- | --- | --- |
| `coakkaCoreNativeDev` | `7c17afc2` | release tooling, docs ledger, and clean-room verification evidence |
| `coakkaCoreNativeDev` | `c5d44657` | inspect Windows archive refresh ledger and release-closure docs |
| `coakka-publish` | `b824bc1` | public artifact surface and release documentation boundary |
| `coakka-publish` | `64589b4` | README points users at the GitHub Release page with attached artifacts |
| `coakka-publish` | `a16bef6` | inspect Windows archives, manifests, checksums, and release notes refreshed to `1.3.1+d7ab7fa` |
| `coakkaJVMConnector` | `feaee7b` | refreshed connector source/package pins over runtime native `1.3.1+bda2ef5` |
| `coakka-samples` | `450ed5d` | public sample repository defaulting the Node.js/Python container path to Docker Hub images |
| `coakka-samples` | `85b2a8c` | post-boundary onboarding docs and `runtime-client docker-bundle` compatibility alias |
| `coakka-samples` | `53ea3a6` | public docs point direct runtime-client downloads at GitHub Release assets |
| `coakka-samples` | `a614565` | public docs embed the runtime-client animated CLI walkthrough GIF |
| `coakka-samples` | `ab5c612` | public Docker Hub image starts native runtime services and drives them with `coakka-client` |
| `coakka-samples` | `bc87a35` | runtime-inspect sample pins/docs resolve all native inspect platforms to `1.3.1+d7ab7fa` |

The artifact source snapshots are intentionally not all the same as the latest
documentation commits. If tags are added later, tag artifact source snapshots
for artifact provenance and tag documentation commits only as release
documentation boundaries.

## Git Tag Boundary

Annotated release boundary tags:

| Repository | Tag | Peeled target commit |
| --- | --- | --- |
| `coakkaCoreNativeDev` | `coakka-runtime-core-v1.3.1-bda2ef5` | `bda2ef53b8b499e6dc27e3d9da51f59800648052` |
| `coakkaCoreNativeDev` | `coakka-runtime-client-v1.3.1-2215b0f` | `2215b0f542010052c5a263e6e0dc21ea4c30b856` |
| `coakkaJVMConnector` | `coakka-runtime-connector-v1.3.1-bda2ef5-0a0aa76` | `0a0aa76d0f2fc7fd8eb7d7e2e5702f4070822deb` |
| `coakka-publish` | `coakka-public-artifacts-v1.3.1` | `f8a70b0295e57b91759567de10b3741bdf315869` |
| `coakka-samples` | `coakka-samples-runtime-v1.3.1` | `85b2a8ca7b4d5a45a86c620d360362229b76254d` |

The `85b2a8c` samples checkpoint does not change the artifact manifest,
published archives, Docker Hub tags, or connector/native package generations.
It clarifies the public first-run path and adds `runtime-client docker-bundle`
as the preferred sample command while retaining the existing `docker-demo`
artifact layout and runner alias for compatibility.

The `53ea3a6`, `a614565`, and `d89701e` samples checkpoints and `64589b4`
publish checkpoint are documentation-only follow-ups after GitHub Release
assets were attached. They do not change artifact provenance, checksums,
Docker Hub tags, or release package generations.

The `ab5c612` samples checkpoint adds the prebuilt Docker Hub
runtime-client demo image. It does not change the canonical archive manifest
or package bytes; it packages the already published Linux Docker bundle into a
one-command sample image.

The inspect closure checkpoints add the browser inspect native archive lane and
Docker Hub sample image after the original runtime-client boundary. macOS ARM64,
Linux x86_64, Linux ARM64, Windows x86_64, and Windows ARM64 now resolve to
`coakka-runtime-inspect` `1.3.1+d7ab7fa`; the Docker Hub inspect image remains
a sample wrapper around the published Linux inspect archives.

The manifest provenance checkpoint updates only comment lines in
`artifacts/public-artifacts.tsv` so public Mojo/Zig source-package rows expose
their connector source commits at the manifest boundary. Artifact files,
per-artifact SHA256 values, source tags, Docker Hub tags, and package
generations are unchanged.

## Docker Hub Boundary

Published multi-arch Docker Hub tags:

```text
docker.io/gabrielgun1983/runtime-base:1.3.1-bda2ef5-remote
docker.io/gabrielgun1983/sample-python-store:1.3.1-bda2ef5-0a0aa76-remote
docker.io/gabrielgun1983/sample-node-web:1.3.1-bda2ef5-0a0aa76-remote
docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote
docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-d7ab7fa-remote
```

Manifest list digests:

```text
runtime-base       sha256:35952461da183b3bb3016d492bdc0459acc8498d022ee48659410982baa3c41c
sample-python-store sha256:7449360692f3248339dbfd0bedb024f03eea8c402ca2e2f06d0e08c608824bcc
sample-node-web     sha256:292148457d64c896b35291a42c51be08d326b82cd735cc883cc915ea1cb3b22f
coakka-runtime-client-demo sha256:ad00c5f990f1d484e3a83deab18f4cac4175d5d41eccb9d27872f342a22e8fe3
coakka-runtime-inspect-sample sha256:95f3a104fc8531d4700d020cad901b5219cbe7ece46175a1fea68bb0d6a63930
```

Each image tag was published for `linux/amd64` and `linux/arm64`.
The runtime-client demo image is a sample convenience image that starts two
native runtime service processes and drives them with the packaged
`coakka-client`; the GitHub Release archives remain the canonical binary
download surface.
The runtime-inspect sample image serves the browser inspect UI from the
published Linux native inspect archives; it is a zero-install sample path, not
the canonical artifact surface.

## Verification Boundary

Verification covered:

- `scripts/verify-public-surface.sh` in `coakka-publish`
- Docker Hub manifest inspection for the original runtime-base,
  sample-python-store, and sample-node-web image tags
- clean-room `coakka-samples` clone at `450ed5d`
- `bash run.sh containers node-python config`
- `bash run.sh containers node-python pull`
- `COAKKA_SAMPLE_EXPECT_RUNTIME_GIT_COMMIT=bda2ef5 bash run.sh containers node-python smoke`
- `bash run.sh runtime-client`
- `COAKKA_PIN_CHECK_NETWORK=1 bash scripts/check-artifact-pins.sh`
- `bash run.sh runtime native basic`
- `bash run.sh runtime python basic`
- `bash run.sh runtime node basic`
- `bash run.sh runtime go basic`

All runtime sample paths reported `version=1.3.1` and runtime git `bda2ef5`.
The Node.js sample emitted only the existing npm `min-release-age` warning.

Follow-up clean-room verification for the `85b2a8c` onboarding checkpoint used
a fresh public clone without a sibling `coakka-publish` checkout, so CLI and
Docker bundle artifacts resolved through the public raw GitHub artifact URLs.
It covered:

- `bash -n run.sh runtime-client/run.sh scripts/sample-metadata.sh scripts/check-artifact-pins.sh scripts/test-artifact-pins.sh`
- `bash run.sh runtime-client --help`
- `bash run.sh list`
- stale public wording scan for early/demo/old runtime version patterns
- `bash run.sh runtime-client`
- `bash scripts/test-artifact-pins.sh`
- `COAKKA_PIN_CHECK_NETWORK=1 bash scripts/check-artifact-pins.sh`
- `bash run.sh containers node-python config`
- `bash run.sh runtime-client docker-bundle`

Follow-up verification for the `ab5c612` runtime-client Docker Hub image
checkpoint covered:

- `docker manifest inspect docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote`
- `docker run --rm docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote`
- `docker run --rm docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-2215b0f-remote client --help`
- `bash run.sh runtime-client dockerhub-demo`
- `bash run.sh runtime-client dockerhub-demo client --help`

Follow-up verification for the inspect native and Docker Hub closure covered:

- macOS ARM64, Linux x86_64, Linux ARM64, Windows x86_64, and Windows ARM64
  inspect native archives at `1.3.1+d7ab7fa`
- Windows x86_64 and Windows ARM64 staged-prefix smokes in the real UTM
  `Windows11` guest
- Windows x86_64 and Windows ARM64 extracted-archive smokes in the same guest
- `scripts/test-public-artifact-manifest.sh`
- `scripts/verify-public-surface.sh`
- `bash run.sh runtime-inspect check`
- `bash run.sh runtime-inspect published-smoke`
- Docker Hub inspect image manifest inspection and direct `linux/amd64` plus
  `linux/arm64` smokes
- GitHub Release download checksum verification for Windows inspect archives
  and root `public-artifacts.tsv`
