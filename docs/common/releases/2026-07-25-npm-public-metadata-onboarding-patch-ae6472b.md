# 2026-07-25 npm Public Metadata And Onboarding Patch ae6472b

This release republishes the JavaScript runtime and logger npm packages with
public package metadata and first-run onboarding text aligned to the fake HTTP
backend to CoAkka target story.

The native runtime and logger generations are unchanged:

- runtime native: `1.3.1+0da8c2d9`
- logger native: `1.2.1+f50756ebff0d`

Published npm coordinates:

- `coakka-v2-connector-node@1.3.7`
- `coakka-v2-connector-bun@1.3.7`
- `coakka-v2-connector-electron@1.3.7`
- `coakka-logger-node@1.2.5`
- `coakka-logger-bun@1.2.5`
- `coakka-logger-electron@1.2.5`

The staged candidate set lives under
`package-manager/npm/candidates/ae6472b/` and passes:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/ae6472b/manifest.json
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/ae6472b/manifest.json
```
