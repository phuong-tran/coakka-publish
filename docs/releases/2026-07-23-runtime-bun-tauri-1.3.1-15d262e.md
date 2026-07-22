# Runtime Bun And Tauri Source Release 1.3.1 15d262e

Date: 2026-07-23

This release adds two runtime connector lanes over native runtime package
generation `1.3.1+bda2ef5`:

- Bun package: `runtime/bun/releases/1.3.1+bda2ef5-15d262e/coakka-v2-connector-bun-1.3.1.tgz`
- Tauri intent source package: `runtime/tauri/releases/1.3.1+bda2ef5-15d262e/coakka-runtime-tauri-intents-1.3.1-source.tar.gz`

The Bun artifact is a Bun package with bundled native runtime libraries. The
Tauri artifact is a source package, not a crates.io release and not a Tauri
plugin. Its public boundary is the Rust app-host intent bridge: WebView
JavaScript sends an intent through Tauri `invoke`, Rust owns execution, and the
WebView receives a projected result.

Both artifacts are attached to the GitHub Release page
`coakka-public-artifacts-v1.3.1`. The attached `public-artifacts.tsv` manifest
checksum after this update is:

```text
a1ea3ed6a090df4a3e05a7ee4d985046448922a00472ce93560fea796f9bfd83  artifacts/public-artifacts.tsv
```

Verification before publish:

```sh
python3 scripts/verify-runtime-intake-artifact.py --lane bun \
  --artifact runtime/bun/releases/1.3.1+bda2ef5-15d262e/coakka-v2-connector-bun-1.3.1.tgz \
  --expected-native-version 1.3.1+bda2ef5

python3 scripts/verify-runtime-intake-artifact.py --lane tauri \
  --artifact runtime/tauri/releases/1.3.1+bda2ef5-15d262e/coakka-runtime-tauri-intents-1.3.1-source.tar.gz \
  --expected-native-version 1.3.1+bda2ef5
```
