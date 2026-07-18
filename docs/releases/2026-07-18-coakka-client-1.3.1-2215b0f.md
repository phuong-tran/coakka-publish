# 2026-07-18 coakka-client 1.3.1 2215b0f

This note records the first public `coakka-client` release lane cut to
`1.3.1+2215b0f`.

`coakka-client` is the CLI runtime client. It is not the dashboard, inspect
surface, or a business schema registry.

| Surface | Artifact root | Version |
| --- | --- | --- |
| Native CLI | `cli/releases/1.3.1+2215b0f/` | `1.3.1+2215b0f` |
| Docker demo | `demo/coakka-client/releases/1.3.1+2215b0f/` | `1.3.1+2215b0f` |

Native CLI platform archives:

- `linux-x86_64`
- `linux-aarch64`
- `macos-aarch64`
- `windows-x86_64`
- `windows-aarch64`

Docker demo bundle archives:

- `linux-x86_64`
- `linux-aarch64`

The release demonstrates:

- offline diagnostics with `version` and `doctor`
- request/reply through `call`
- `ask` as the request/reply alias
- JSON payload shorthands and metadata output
- bounded shell script mode through `shell --script`
- zero-install Docker demo round-trips for Linux x86_64 and Linux ARM64

Validation covered:

- fresh macOS ARM64, Linux x86_64/ARM64, and Windows x86_64/ARM64 CLI prefixes
  built from source snapshot `2215b0f`
- staged macOS/Linux `coakka-client version --output json` and
  `doctor --output json` smokes reporting `runtime_version=1.3.1`
- UTM `Windows11` `version --output json` and `doctor --output json` smokes
  for Windows x86_64 and Windows ARM64, both with exit code `0`
- public CLI release verifier over all five archives, manifests, dependency
  gates, and SHA256 checks
- Docker demo release verifier over both Linux bundles, manifests, dependency
  gates, and SHA256 checks
- Docker Compose live verifier for both Linux demo bundles

Current limits:

- request-driving uses the public TCP frame profile
- topology and capability truth stay runtime-owned
- message type and schema version are caller metadata, not schema validation
- Docker is the evaluation/demo lane, not the source of the macOS video
  recording
