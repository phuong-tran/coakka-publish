# CoAkka Runtime Addons

`runtime-addons/` is the release family for optional native capabilities that
compose with CoAkka Runtime without becoming part of the default runtime
package.

An addon release is an independent artifact. It must not bundle another copy
of CoAkka Runtime, and the default runtime package must not acquire the addon's
implementation dependencies. Each addon manifest declares the runtime ABI,
minimum runtime package version, and required runtime features it consumes.

## Why Artifact Source Addons Exist

File Lane transfers a stable local file between CoAkka peers. Real workflows
often begin before that file exists locally: an AI model may live at one pinned
Hugging Face commit, a checkpoint in one S3 object version, or a diagnostic
bundle behind SFTP or an immutable HTTPS URL.

The current Artifact Source Addons, also described as file acquisition
providers or provider-specific downloaders, own that upstream step:

```text
external provider identity
  -> authenticate and acquire
  -> verify exact size and SHA-256
  -> stage locally without replacement
  -> CoAkka File Lane
  -> destination service
```

This keeps provider SDKs, credentials, redirect and retry rules, and remote
identity semantics out of Runtime core. It also avoids creating a private HTTP
file endpoint with custom body limits, temporary-file cleanup, integrity,
resume, cancellation, and receiver-completion behavior for every service.
HTTP remains appropriate for public/browser distribution and CDN-backed files;
File Lane is the bounded point-to-point path for application-owned peers.

If a file already exists locally, use File Lane directly; no source addon is
needed. Read [Runtime Addons](../docs/runtime-addons.md) for the AI-era use
case, provider selection, HTTP comparison, and ownership boundary.

## Current Language Boundary

Released addons currently expose native C ABIs and native C11 consumers only.
The ABI is ready for JVM, Python, Node.js, Go, .NET, Swift, and other language
wrappers without rewriting the provider engines, but no such high-level addon
connector is currently released. Each connector still needs ownership-safe
bindings, credential/error mapping, packaging, matching-host tests, and ongoing
maintenance, so this work remains demand-driven.

## Release Layout

```text
runtime-addons/<addon>/native/releases/<release>/
  coakka-runtime-addon-<addon>-native-<version>.tar.gz
  manifest.json
  SHA256SUMS
  README.md
```

The archive contains its public C ABI header, one native module for every
declared platform, CMake package metadata, addon metadata, and third-party
license notices. Implementation libraries such as protocol or crypto providers
must be absorbed into the addon when their licenses permit it. Consumers must
not be asked to install ambient native dependencies.

## Versioning And Compatibility

Addon versions advance independently from the runtime package. Compatibility
is explicit rather than inferred from matching version numbers:

- `runtimeCompatibility.abiMajor` identifies the required runtime ABI family;
- `runtimeCompatibility.minimumNativeVersion` identifies the oldest supported
  native runtime package;
- `runtimeCompatibility.requiredFeatures` names public runtime capabilities
  used by the addon;
- every listed platform carries matching-host runtime-test and dynamic-linkage
  evidence.

An addon archive never widens or replaces runtime semantics. Runtime owns
routing, lifecycle, File Lane behavior, deadletters, and diagnostics. The addon
owns only its external capability workflow.

## Promotion Rule

A directory or package template is not a public release. Promotion requires:

1. an immutable archive and release manifest;
2. matching `SHA256SUMS`;
3. a passing `scripts/verify-runtime-addon-release.py` intake;
4. matching-host runtime-module tests for every declared platform;
5. dynamic dependency inspection proving no user-installed implementation
   library is required;
6. one `artifacts/public-artifacts.tsv` row and synchronized current-package
   documentation.

Until all six are present for a platform, documentation must describe that
platform as a source candidate or release template, not as supported by a
public artifact.

## Current Lanes

- The 11-addon artifact-source wave is public at native
  `1.1.0+d1032f6d`. HTTPS, S3/MinIO, Azure Blob, GCS, WebDAV, OCI
  Distribution, Hugging Face Hub, GitHub release assets, Google Drive, and
  Dropbox include five native targets. Local Drop includes Linux ARM64,
  Linux x86-64, and macOS ARM64 because its anchored filesystem contract is
  POSIX-only.
- [`artifact-publisher-sftp`](artifact-publisher-sftp/README.md): replacement
  native `1.2.0+88b9a047` for Linux ARM64/x86-64, macOS ARM64, and Windows
  11 ARM64/x86-64. The withdrawn `1.1.0+42841ae2` coordinate is not retained.

Each addon directory contains its exact archive coordinate, ownership model,
platform set, Runtime compatibility, and current connector limitation.
