# CoAkka Runtime Addons

`runtime-addons/` is the release family for optional native capabilities that
compose with CoAkka Runtime without becoming part of the default runtime
package.

An addon release is an independent artifact. It must not bundle another copy
of CoAkka Runtime, and the default runtime package must not acquire the addon's
implementation dependencies. Each addon manifest declares the runtime ABI,
minimum runtime package version, and required runtime features it consumes.

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

- [`artifact-publisher-sftp`](artifact-publisher-sftp/README.md): native
  `0.1.0+40810b79` is public for `macos-aarch64`; Linux awaits matching-host
  package evidence and Windows remains blocked by staging safety.
