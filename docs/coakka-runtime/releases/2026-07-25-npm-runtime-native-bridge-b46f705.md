# npm Runtime Package Candidate b46f705

This note records the npm-ready runtime connector candidate for the runtime
package release.

- `coakka-v2-connector-node@1.3.9`
- `coakka-v2-connector-bun@1.3.9`
- `coakka-v2-connector-electron@1.3.9`

The candidate bundles native runtime generation `1.3.2+caff6d6d` and is built
from connector commit `b46f705`. The JavaScript package surface no longer ships
the old userland transport codec; the runtime package boundary is provided by
the bundled native runtime.

Candidate directory:

```sh
package-manager/npm/candidates/b46f705/
```

Validation:

```sh
scripts/test-npm-package-manager-candidates.sh \
  --require-public-metadata \
  package-manager/npm/candidates/b46f705/manifest.json
```

After npm publish, verify the registry surface with:

```sh
scripts/verify-npm-registry-candidates.sh \
  --manifest package-manager/npm/candidates/b46f705/manifest.json
```
