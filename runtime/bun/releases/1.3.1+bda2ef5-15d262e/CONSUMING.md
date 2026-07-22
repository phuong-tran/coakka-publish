# Consuming `coakka-v2-connector-bun`

Install the tarball with Bun:

```sh
bun add /path/to/coakka-v2-connector-bun-1.3.1.tgz
```

Use the Bun-named API surface:

```js
import {
  BunRuntimeHost,
  PayloadIdentity,
  localRoute,
} from "coakka-v2-connector-bun";
```

Current embedded native package:

- native core version: `1.3.1`
- native package version: `1.3.1+bda2ef5`
- platforms: `macos-aarch64`, `linux-aarch64`, `linux-x86_64`,
  `windows-aarch64`, `windows-x86_64`

The package shares the runtime connector core with the Node.js package, but it
is packaged and smoked as a Bun consumer artifact.
