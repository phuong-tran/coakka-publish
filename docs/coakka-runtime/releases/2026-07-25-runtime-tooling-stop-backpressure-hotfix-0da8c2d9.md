# Runtime Tooling Stop Backpressure Hotfix 0da8c2d9

This note records the native `coakka-client` and `coakka-runtime-inspect`
refresh over CoAkka Runtime source snapshot `0da8c2d9`.

The underlying runtime fix closes runtime-owned terminal write lanes before
joining outbound writers during `runtime_stop()`. This prevents stop from
blocking when terminal output backpressure is present.

## Artifacts

| Lane | Release root | Generation |
| --- | --- | --- |
| `coakka-client` native CLI | `cli/releases/1.3.1+0da8c2d9/` | `1.3.1+0da8c2d9` |
| `coakka-runtime-inspect` native UI | `runtime-inspect/native/releases/1.3.1+0da8c2d9/` | `1.3.1+0da8c2d9` |
| `coakka-client` Docker Linux bundle | `demo/coakka-client/releases/1.3.1+0da8c2d9/` | `1.3.1+0da8c2d9` |

The refreshed native tooling archives cover:

- `linux-x86_64`
- `linux-aarch64`
- `macos-aarch64`
- `windows-x86_64`
- `windows-aarch64`

The Docker Hub sample images are published separately as:

```text
docker.io/gabrielgun1983/coakka-runtime-client-demo:1.3.1-0da8c2d9-remote
docker.io/gabrielgun1983/coakka-runtime-inspect-sample:1.3.1-0da8c2d9-remote
```

Docker Hub manifest-list digests:

```text
coakka-runtime-client-demo:1.3.1-0da8c2d9-remote sha256:a71536289fbccd68467738ea3c9a4802b1376d3ac19365e477499a7fecc05726
coakka-runtime-inspect-sample:1.3.1-0da8c2d9-remote sha256:b83a38bd8c7de5ee73e0b4cf6584ec03e31bfcbbac62b3c2870c43c7cca0b49f
```

## Checksums

```text
5d7777233ab73d62eb7c089860e09d9093e89c6571d8964e33a1b4525a70f50d  coakka-client-v2-1.3.1-linux-x86_64.tar.gz
8336084dc56aacd496c5659dc99d9326b839f9b8b7342a52f32d391fa882e884  coakka-client-v2-1.3.1-linux-aarch64.tar.gz
8b4b463bd4a77997cb506f4c051505a7111a5b4736f11cf4e6bce92d92881398  coakka-client-v2-1.3.1-macos-aarch64.tar.gz
6a7c0ae73aa68ee30c2c009f1c06c5238a769620b681ffcef25dc4a0fc3af02f  coakka-client-v2-1.3.1-windows-x86_64.tar.gz
be2f91234cf82e6aa4bf02c7d9881836439d3f7bafbd0eab252975571ccec963  coakka-client-v2-1.3.1-windows-aarch64.tar.gz
faef8b1b83d09a70b5158a5ca92a601d790e633d72de412cf5803ec79c974616  coakka-runtime-inspect-v2-1.3.1-linux-aarch64.tar.gz
71f8ee17bfe24015a37c4f06844954b6edeb2a25b10ce3aacef99097c9db9e6a  coakka-runtime-inspect-v2-1.3.1-linux-x86_64.tar.gz
6817ed2213239f0c38d049dd354fc940b738824b46ec3a96116b5098e7d2a656  coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz
c87aea00d5ce0b082cd9a3141cba808c2d5a9b04547496eb559a496745b6ab86  coakka-runtime-inspect-v2-1.3.1-windows-aarch64.tar.gz
60a4cefa65300e9d13548f181a19cd3c1b88e6d5559c1f7771e79c649be5a1b9  coakka-runtime-inspect-v2-1.3.1-windows-x86_64.tar.gz
075539f715dc5d9bc52bdfaa9dc652f97634af178b90df69ace82731cbcabe64  coakka-client-docker-demo-v2-1.3.1-linux-x86_64.tar.gz
d0440bdbd8824f7a1e6e801b29cd14abe79664d885089b0859fa82406cb20e12  coakka-client-docker-demo-v2-1.3.1-linux-aarch64.tar.gz
```

## Verification

- `release_public_coakka_client.sh --source-snapshot 0da8c2d9`
- `release_coakka_runtime_inspect_native_bundle.sh --source-snapshot 0da8c2d9`
  for macOS ARM64
- Windows x86_64 and Windows ARM64 inspect cross-builds with dependency gates
- Linux x86_64 and Linux ARM64 inspect Docker builds with command and serve
  smokes
- `verify_public_coakka_client_release.sh`
- `verify_coakka_runtime_inspect_native_bundle.sh`
- `verify_public_coakka_client_docker_demo_release.sh --live`
- `docker manifest inspect` for both Docker Hub sample images
- `runtime-client dockerhub-demo` and `runtime-inspect dockerhub-smoke`
