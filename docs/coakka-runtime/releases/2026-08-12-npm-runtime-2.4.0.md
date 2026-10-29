# npm Runtime 2.4.0

CoAkka published and registry-verified the Node.js, Bun, and Electron runtime
packages at `2.4.0` on August 12, 2026:

- `coakka-v2-connector-node@2.4.0`
- `coakka-v2-connector-bun@2.4.0`
- `coakka-v2-connector-electron@2.4.0`

The packages use connector source `0afb5e9`, payload staging commit `ef40ada`,
and native runtime generation `2.4.0+c2f53117`. Node.js and Bun carry the
five-platform native payload; Electron requires exact Node.js `2.4.0`.

This release adds explicit embedded, outbound-only, and network-node
participation modes while retaining File Lane, Stream Lane, request/reply,
bounded admission, deadletters, and the existing lifecycle contract.

The immutable receipt and staged tarballs are under
`package-manager/npm/candidates/0afb5e9/`. Registry verification confirms the
version, `latest` tag, tarball SHA-1 and integrity, native generation, platform
contents, and Electron dependency. Clean registry installs complete native
request/reply on Node.js and Bun and the Electron main-process intent path.
