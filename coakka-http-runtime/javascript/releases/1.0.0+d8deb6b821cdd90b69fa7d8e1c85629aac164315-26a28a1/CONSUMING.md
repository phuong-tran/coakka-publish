# Consuming The JavaScript Tarball

Install the downloaded tarball without contacting the npm registry:

```sh
npm install /path/to/coakka-http-1.0.0.tgz
```

```javascript
import { Builder, Response } from "@coakka/http";

const service = new Builder()
  .get("/health", () => Response.empty())
  .start();
try {
  console.log(service.port);
} finally {
  await service.close();
}
```

The loader chooses the process OS/ISA prebuild, verifies its native manifest,
loads the adjacent Core and then loads the Node-API addon. Use `Builder` and
`Service` for buffered request/reply or `createCore` for the complete low-level
surface. Low-level take/wait methods are synchronous; poll with zero timeout or
use a dedicated Worker rather than blocking the main event loop.
