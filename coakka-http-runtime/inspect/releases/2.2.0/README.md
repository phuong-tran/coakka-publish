# CoAkka HTTP Runtime Inspect 2.2.0

This standalone release includes CoAkka HTTP Runtime Core 2.2. Verify the
archive against [SHA256SUMS](SHA256SUMS) before extracting it.

| Target | Archive |
| --- | --- |
| macOS ARM64 | [Download](coakka-http-runtime-inspect-2.2.0-macos-aarch64.tar.gz) |
| Linux ARM64 (glibc 2.28 or later) | [Download](coakka-http-runtime-inspect-2.2.0-linux-aarch64.tar.gz) |

After extraction, run `./bin/coakka-http-runtime-inspect --version` and
expect `coakka-http-runtime-inspect 2.2.0 assets=3`. The macOS binary uses an
ad hoc signature. See the [installation and connection guide](../../README.md).

The packaged application passed startup, connection and refusal, OpenAPI
export, Route Try, disconnect and graceful shutdown against a standalone
service using Core 2.2. The macOS archive ran on macOS ARM64. The Linux
archive ran on Raspberry Pi 5 and in a glibc 2.28 environment.
