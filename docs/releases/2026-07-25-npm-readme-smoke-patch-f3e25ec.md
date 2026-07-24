# 2026-07-25 npm README Smoke Patch f3e25ec

This release republishes the JavaScript runtime and logger npm packages so the
npm package README pages link to the no-checkout first npm smoke guide.

The native runtime and logger generations are unchanged:

- runtime native: `1.3.1+bda2ef5`
- logger native: `1.2.1+f50756ebff0d`

Published npm coordinates:

- `coakka-v2-connector-node@1.3.4`
- `coakka-v2-connector-bun@1.3.4`
- `coakka-v2-connector-electron@1.3.4`
- `coakka-logger-node@1.2.3`
- `coakka-logger-bun@1.2.3`
- `coakka-logger-electron@1.2.3`

The staged candidate set lives under
`package-manager/npm/candidates/f3e25ec/` and passes:

```sh
scripts/test-npm-package-manager-candidates.sh
scripts/verify-npm-registry-candidates.sh
```
