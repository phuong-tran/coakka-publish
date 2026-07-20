# 2026-07-20 CoAkka Runtime Inspect Docker Hub Image 1.3.1 4ce41f19

This note records the public Docker Hub sample image refresh for
`coakka-runtime-inspect`.

Image:

```text
docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote
```

Manifest digest:

```text
sha256:0f69c5eb4eac4ea2c11c5123971a591bf67adb4d6c0f791283f3286d3aecca89
```

Platforms:

- `linux/amd64`
- `linux/arm64`

The image is a sample convenience wrapper around the published Linux native
inspect archives at `1.3.1+4ce41f19`. Its Dockerfiles do not install native
implementation runtime packages.

## Verification

Verified with:

- `docker buildx imagetools inspect docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote`
- `docker run --rm --platform linux/amd64 docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote smoke`
- `docker run --rm --platform linux/arm64 docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-4ce41f19-remote smoke`
- `bash run.sh runtime-inspect docker-smoke` from `coakka-samples`
- `bash run.sh runtime-inspect dockerhub-smoke` from `coakka-samples`

`coakka-runtime-inspect` remains the browser runtime explorer. It is not an
admin dashboard, schema registry, service discovery server, mTLS control plane,
or business schema owner.
