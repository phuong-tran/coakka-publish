# 2026-07-25 npm Onboarding Patch e5d3342

This release republishes the JavaScript runtime and logger npm packages with
clearer first-reader README content and links back to the public repositories:

- `coakka-publish`: https://github.com/phuong-tran/coakka-publish
- `coakka-samples`: https://github.com/phuong-tran/coakka-samples

The native runtime and logger generations are unchanged:

- runtime native: `1.3.1+bda2ef5`
- logger native: `1.2.1+f50756ebff0d`

Published npm coordinates:

- `coakka-v2-connector-node@1.3.2`
- `coakka-v2-connector-bun@1.3.2`
- `coakka-v2-connector-electron@1.3.2`
- `coakka-logger-node@1.2.2`
- `coakka-logger-bun@1.2.2`
- `coakka-logger-electron@1.2.2`

The staged candidate set lives under
`package-manager/npm/candidates/e5d3342/` and passes:

```sh
scripts/test-npm-package-manager-candidates.sh
scripts/verify-npm-registry-candidates.sh
```
