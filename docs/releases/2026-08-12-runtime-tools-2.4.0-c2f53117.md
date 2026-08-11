# CoAkka Runtime Tools 2.4.0

Date: 2026-08-12

This release publishes `coakka-client` and `coakka-runtime-inspect` from Core
source snapshot `c2f53117f991f67f809a0bf46bac2ce26091eb78` as native generation
`2.4.0+c2f53117`.

## Artifacts

Both tools are available for:

- macOS ARM64
- Linux ARM64
- Linux x86-64
- Windows ARM64
- Windows x86-64

The immutable artifact commit is
`da4a5e9c3f1f846970fb84c8f18bca893051c487`. Exact archive paths and SHA-256
digests are recorded in `artifacts/public-artifacts.tsv`; each release
directory also carries `SHA256SUMS` and its package manifest.

## Verification

- all ten archives pass archive-layout, checksum, binary-format, architecture,
  and native-dependency gates
- macOS ARM64 `coakka-client` command gates pass; Linux ARM64/x86-64 pass
  matching-architecture Docker build and dependency gates
- macOS ARM64 and Linux ARM64/x86-64 `coakka-runtime-inspect` command and
  `serve` smokes pass on matching hosts
- Windows ARM64/x86-64 packages pass Zig cross-build, PE architecture,
  dependency, archive, and checksum gates

Matching-host Linux command execution is not recorded for `coakka-client`, and
matching-host Windows execution is not recorded for either tool. The Windows
binaries are unsigned; consumers may see normal SmartScreen or local security
policy warnings.

These artifacts are repository-hosted files in `coakka-publish`. No GitHub
Release is created for `coakka-publish` or `coakka-samples`.
