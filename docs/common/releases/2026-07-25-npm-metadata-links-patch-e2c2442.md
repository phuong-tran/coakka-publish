# 2026-07-25 npm Metadata Links Patch e2c2442

This release republishes the JavaScript runtime and logger npm packages with
npm metadata links for source, samples, issues, and package discovery.

The native runtime and logger generations are unchanged:

- runtime native: `1.3.1+bda2ef5`
- logger native: `1.2.1+f50756ebff0d`

Published npm coordinates:

- `coakka-v2-connector-node@1.3.5`
- `coakka-v2-connector-bun@1.3.5`
- `coakka-v2-connector-electron@1.3.5`
- `coakka-logger-node@1.2.4`
- `coakka-logger-bun@1.2.4`
- `coakka-logger-electron@1.2.4`

The staged candidate set lives under
`package-manager/npm/candidates/e2c2442/` and passes:

```sh
scripts/test-npm-package-manager-candidates.sh
scripts/verify-npm-registry-candidates.sh
```
