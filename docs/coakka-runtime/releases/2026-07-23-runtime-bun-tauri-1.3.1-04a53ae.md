# Runtime Bun And Tauri Source Release 1.3.1 04a53ae

Date: 2026-07-23

This release refreshes the Bun and Tauri runtime connector lanes over native
runtime package generation `1.3.1+bda2ef5`.

- Bun package: `runtime/bun/releases/1.3.1+bda2ef5-04a53ae/coakka-v2-connector-bun-1.3.1.tgz`
- Tauri intent source package: `runtime/tauri/releases/1.3.1+bda2ef5-04a53ae/coakka-runtime-tauri-intents-1.3.1-source.tar.gz`

The Bun artifact is a Bun package with bundled native runtime libraries. The
Tauri artifact is a source package, not a crates.io release and not a Tauri
plugin. Its public boundary is the Rust app-host intent bridge: WebView
JavaScript sends an intent through Tauri `invoke`, Rust owns execution, and the
WebView receives a projected result.

This refresh keeps public package contents at the CoAkka boundary:

- no public package path exposes runtime wire schema file names
- no public package source text exposes native implementation dependency names
- normal consumers do not install external native runtime dependencies

Both artifacts are attached to the GitHub Release page
`coakka-public-artifacts-v1.3.1`. The attached `public-artifacts.tsv` manifest
checksum after this update is:

```text
ae71432feb6b8df9fae3c33739dec61f329267f6d5e7bcc13f77e61a976fc93d  artifacts/public-artifacts.tsv
```

Verification before publish:

```sh
python3 scripts/verify-runtime-intake-artifact.py --lane bun \
  --artifact runtime/bun/releases/1.3.1+bda2ef5-04a53ae/coakka-v2-connector-bun-1.3.1.tgz \
  --expected-native-version 1.3.1+bda2ef5

python3 scripts/verify-runtime-intake-artifact.py --lane tauri \
  --artifact runtime/tauri/releases/1.3.1+bda2ef5-04a53ae/coakka-runtime-tauri-intents-1.3.1-source.tar.gz \
  --expected-native-version 1.3.1+bda2ef5
```
