# CoAkka Tools

`coakka-tools` hosts the published artifacts for companion tools. For Runtime,
choose a terminal workflow or a browser workflow:

| Product name | Purpose | Guide |
| --- | --- | --- |
| **CoAkka Runtime Client** — `coakka-runtime-client` | Try Runtime requests from a terminal or script. | [Client guide](../docs/coakka-runtime/operations/coakka-runtime-client.md) |
| **CoAkka Runtime Inspect** — `coakka-runtime-inspect` | Explore Runtime facts and try request/reply in a browser. | [Inspect guide](../docs/coakka-runtime/operations/coakka-runtime-inspect.md) |

The published Client command and archive prefix are `coakka-client`. Use that
command in the current release's examples; `coakka-runtime-client` is the
product name. Inspect's command is already `coakka-runtime-inspect`.

The Client also has `coakka-client/docker-demo` bundles for Linux walkthroughs.
Tools keep their own release versions; use each guide's exact package and scope.

The current checksummed artifact list lives in
[`../artifacts/public-artifacts.tsv`](../artifacts/public-artifacts.tsv).
Older release paths such as `cli/releases/`, `demo/coakka-client/releases/`,
and `runtime-inspect/native/releases/` remain in the repository as historical
compatibility paths.
