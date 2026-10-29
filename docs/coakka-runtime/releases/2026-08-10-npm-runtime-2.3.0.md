# npm Runtime 2.3.0

CoAkka published and registry-verified the Node.js, Bun, and Electron runtime
packages at `2.3.0` on August 10, 2026:

- `coakka-v2-connector-node@2.3.0`
- `coakka-v2-connector-bun@2.3.0`
- `coakka-v2-connector-electron@2.3.0`

The packages use connector source `3a84c7b` and native runtime generation
`2.3.0+a83ab412`. Node.js and Bun carry the five-platform native payload;
Electron requires exact Node.js `2.3.0`.

This release makes Stream Lane available through the npm connector packages.
It also retains File Lane, request/reply, bounded admission, deadletters, and
the existing runtime lifecycle contract. CoAkka reports pressure and terminal
state without choosing application-specific adaptation policy.

The immutable receipt and staged tarballs are under
`package-manager/npm/candidates/3a84c7b/`. Registry verification compares
version, dist-tag, tarball digest and integrity, native generation, platform
contents, and the Electron dependency with that receipt.
