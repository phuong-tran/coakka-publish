# Inspect, route metadata and OpenAPI

Inspect is an optional local development companion to CoAkka HTTP Runtime.
It answers two different questions: **what routes are active now?** and
**what does the application say those routes accept and return?**
It is not required to serve HTTP and does not replace application validation.

## Contents

- [Why Inspect exists](#why-inspect-exists)
- [Inspect versus Swagger and OpenAPI](#inspect-versus-swagger-and-openapi)
- [Connection configuration versus route metadata](#connection-configuration-versus-route-metadata)
- [What metadata describes](#what-metadata-describes)
- [Current package integration](#current-package-integration)
- [OpenAPI projection](#openapi-projection)
- [Using the document with Swagger UI](#using-the-document-with-swagger-ui)
- [Refresh and safety](#refresh-and-safety)

## Why Inspect exists

In a polyglot application, endpoint descriptions should not require a different
inspection model for every host. Inspect joins Core-issued effective route
snapshots with explicitly supplied application schema metadata, then presents
a local browser view. Structural changes and handler-binding revisions remain
observable without inspecting handler classes, source code or live payloads.

The standalone application owns its browser UI, cache, OpenAPI projection and
route-try client. The target owns effective route truth. Metadata is separate
from request dispatch: descriptions and examples do not belong in every HTTP
exchange. Stopping or refreshing Inspect does not reconfigure the target.

## Inspect versus Swagger and OpenAPI

| Name | Role | Relationship to CoAkka |
| --- | --- | --- |
| OpenAPI | A language-neutral HTTP API description specification | Inspect projects an OpenAPI 3.0.3 JSON document. |
| Swagger UI | A tool that renders an OpenAPI document and can issue API requests | It can consume the exported document; it does not provide CoAkka's live route-snapshot connection. |
| CoAkka HTTP Runtime Inspect | A local viewer of effective routes, explicit metadata and cached state, with route try | It provides the runtime-aware view and the OpenAPI export; it does not embed Swagger UI. |

OpenAPI is the format, not a particular browser application. Swagger UI accepts
a document through `url` or `spec`; see the
[official Swagger UI configuration](https://swagger.io/docs/open-source-tools/swagger-ui/usage/configuration/).
The standard used here is [OpenAPI 3.0.3](https://spec.openapis.org/oas/v3.0.3.html),
not Swagger 2.0 or an assertion of OpenAPI 3.1/3.2 support.

Inspect's runtime-aware view and Swagger UI are complementary. A generic
OpenAPI renderer does not reproduce route-generation observation, connection
state or CoAkka-specific extension behavior simply by loading the JSON.

## Connection configuration versus route metadata

| Configuration | Owner and purpose |
| --- | --- |
| Target inspection opt-in and bearer acceptance | Target Core's bounded authenticated read surface; disabled unless enabled explicitly. |
| Target instance/generation identity and read budgets | Identify the observed runtime and bound snapshot work and response retention. |
| Inspect browser address, target address and bearer-file path | Standalone Inspect startup/connect configuration; see the [installation guide](README.md#connect-to-a-service). |
| Route API metadata | Application-owned descriptions joined to a route by stable route ID. |

The bearer used for inspection reads is not an API user's credential and must
not become an example header or an OpenAPI security token. The inspection port
is not necessarily the application's serving port or its public URL.

## What metadata describes

The following is a **conceptual authoring checklist**, not YAML to load or an
invented Kotlin builder. Exact publication calls depend on the package surface.

For a declared `GET /customers/{id}` route:

| Metadata | Example intent | Rule |
| --- | --- | --- |
| Route identity | Stable ID of this customer route | Must refer to the real route, not an unrelated handler name. |
| Summary/description/tags | Read a customer; tag `customers` | Public-safe application prose, bounded in size. |
| Path parameter | `id`, required string | Name must agree with the `{id}` capture. |
| Query parameter | Optional `verbose` boolean | Describes a query value; does not change method/path routing. |
| Request body | For a create/update operation, an explicit media type and schema | Do not infer a body schema from observed traffic. |
| Responses | `200` with a customer schema; described error responses | Describe what the handler actually returns; metadata does not implement it. |
| Schema definitions | Object properties, required fields, arrays, references and formats | Keep definitions consistent and within supported bounds. |
| Examples | Public-safe example values | Opaque example bytes have the export treatment described below. |
| Security requirements | Application-declared requirements | Documentation does not enforce authorization or supply secret credentials. |

Omitting metadata leaves a route routable and explicitly undocumented. Invalid
supplied metadata must not partially publish. Metadata is not inferred from
Kotlin/Java reflection, Python annotations, Go structs or request samples.
The app remains responsible for validation, serialization and authorization.

## Current package integration

**Core already supports route API metadata; the language connectors do not yet
expose its end-user declaration API.** Inspect can consume Core's metadata and
project OpenAPI. The planned connector work connects to that existing capability,
with examples and tests; it is not a new Core metadata design.

The current native archive exposes `coakka_http_inspection_options_t` and
`coakka_http_configuration_set_inspection` for target inspection setup. Its
options cover bearer-file selection, runtime identity, call capacity, per-turn
entry/byte budgets, snapshot deadline and response-byte ceilings. Use its
initializer and checked configuration result; an inspection endpoint must be
explicitly enabled, never silently added to an ordinary service.

The current language feature samples do not enable this target surface.
The checked connector sources do not yet provide a documented host-inlined
metadata-publication recipe, and the installed native header does not expose
a route-schema authoring API. Route snapshot access and metadata-generation
fields alone are not such an API. Therefore this guide does not present a
fictional `.metadata(...)` call for Kotlin, Java, Go, Python or JavaScript.

An already inspection-enabled target can use the packaged Inspect workflow.
Adding end-user metadata authoring to the connector guides requires a public
contract and artifact-backed examples first; see the [roadmap](../roadmap.md).
Do not reach into private headers or schema encodings to compensate.

## OpenAPI projection

After a successful connection and snapshot load, Inspect serves
`GET /openapi.json` on **its own browser listener**. This reads the local cache,
not one target query per browser request. Before a usable model is available,
the endpoint can return `503`; treat it as a failed export, not an empty API.

| Document part | Projection behavior |
| --- | --- |
| Version | `openapi: "3.0.3"` |
| Paths and operations | Standard HTTP methods are grouped by route pattern; custom method routes are retained in `x-coakka-custom-method-routes`. |
| Operation identity | Standard `operationId` derives from stable route ID; a supplied suggested name is retained in `x-coakka-operation-id`. |
| Schemas | Route-local definitions become distinct component names with rewritten references. |
| Undocumented routes | Remain visible with a minimal default response and documentation-state marker; no business schema is invented. |
| Examples | Opaque bytes are preserved as hex in `x-coakka-value-hex`, not injected as arbitrary JSON examples. |
| Authentication descriptions | Preserved in `x-coakka-security-requirements`; standard security schemes are not invented from incomplete transport information. |
| Application origin | No `servers` entry is inferred from the inspection address. |
| Runtime state | `x-coakka-*` extensions retain identity, generation and stale-state information. |

This is a bounded OpenAPI projection, not an automatic description of every
business behavior or a compatibility certificate for all OpenAPI tools.
The recorded Inspect checks cover projection and its own browser workflow;
they do not establish an end-to-end Swagger UI execution matrix.

## Using the document with Swagger UI

With Inspect running at the example local browser port and a snapshot loaded:

```sh
curl --fail --max-time 5 http://127.0.0.1:8080/openapi.json -o coakka-openapi.json
```

Keep the export private unless reviewed for publication. Load the resulting
document into a locally hosted Swagger UI using its `spec` option, or serve a
reviewed copy at a URL that Swagger UI can access. A reading-only configuration
can use `supportedSubmitMethods: []` and `validatorUrl: null` to avoid enabling
Try It Out or sending a private specification to the default online validator.
Hosting/version selection for Swagger UI is your application's tooling setup,
not a bundled Inspect dependency.

Before enabling Swagger UI's Try It Out, provide the correct application
`servers` URL and review security-scheme declarations in the deployment-owned
copy. Without an explicit server, relative requests can target the document/UI
origin instead of the application. Configure application authentication and,
if origins differ, the necessary browser CORS policy. Do not publish Inspect's
loopback-only listener to solve a browser access problem. Its bearer token is
not application authorization. Keep deployment enrichment separate from the
runtime-generated document so the source of each declaration remains clear.

## Refresh and safety

Refresh pulls and publishes a new local view; it does not hot-swap the target's
handler or upload edited OpenAPI back into the service. Inspect is not an
OpenAPI-to-runtime configuration importer. Cached views can become stale:
check status and generation before assuming they describe the current routes.
Route Try is a real request and may change state; it is not a dry run. Do not
retry a timed-out state-changing request without application idempotency rules.
