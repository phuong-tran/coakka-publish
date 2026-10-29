# 2026-07-19 CoAkka Runtime Inspect Docker Hub Image 1.3.1 d7ab7fa

This note records the public Docker Hub sample image for
`coakka-runtime-inspect`.

Image:

```text
docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-d7ab7fa-remote
```

Manifest digest:

```text
sha256:95f3a104fc8531d4700d020cad901b5219cbe7ece46175a1fea68bb0d6a63930
```

Platforms:

- `linux/amd64`
- `linux/arm64`

The image is a sample convenience wrapper around the published Linux native
inspect archives. The canonical binary artifact surface remains
`runtime-inspect/native/releases/` plus the public manifest and checksums.

## Verification

Verified with:

- `docker buildx imagetools inspect docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-d7ab7fa-remote`
- `docker run --rm --platform linux/amd64 docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-d7ab7fa-remote smoke`
- `docker run --rm --platform linux/arm64 docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-d7ab7fa-remote smoke`
- `bash run.sh runtime-inspect dockerhub-smoke` from `coakka-samples`
- Docker Hub serve smoke for `/healthz` and `/api/snapshot`

`coakka-runtime-inspect` remains the browser runtime explorer and route-try UI.
It is not an admin dashboard, schema registry, service discovery server, mTLS
control plane, or business schema owner.
