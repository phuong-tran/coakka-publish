# npm Package-Manager Release 55bbeb7

Date: 2026-07-24

This release opens the npm package-manager lane for the JavaScript runtime and
logger surfaces. The published packages were built from connector commit
`55bbeb7` and registry-verified against the staged tarballs under
`package-manager/npm/candidates/55bbeb7/`.

## Published Coordinates

| Surface | npm coordinate | Native generation |
| --- | --- | --- |
| Node.js runtime | `coakka-v2-connector-node@1.3.1` | `1.3.1+bda2ef5` |
| Bun runtime | `coakka-v2-connector-bun@1.3.1` | `1.3.1+bda2ef5` |
| Electron runtime | `coakka-v2-connector-electron@1.3.1` | via `coakka-v2-connector-node@1.3.1` |
| Node.js logger | `coakka-logger-node@1.2.1` | `1.2.1+f50756ebff0d` |
| Bun logger | `coakka-logger-bun@1.2.1` | `1.2.1+f50756ebff0d` |
| Electron logger | `coakka-logger-electron@1.2.1` | via `coakka-logger-node@1.2.1` |

## Verification

The npm registry verification gate checks the published version, license,
dependency metadata, tarball shasum, and `latest` dist-tag for every package:

```sh
scripts/verify-npm-registry-candidates.sh
```

The package-manager artifact gate remains in CI and verifies the staged
candidate tarballs have registry-ready metadata, self-contained native delivery,
and no raw/file/git dependency URLs.
