# 2026-07-05 Logger Mojo And Zig Source Refresh 1107154

This note records the first public source connector packages for the Mojo and
Zig logger lanes over native logger package generation `0.1.0+ba2a66d98eb5`.

| Surface | Public artifact | Version | Native package |
| --- | --- | --- | --- |
| Mojo logger source connector | `logger/mojo/releases/0.1.0+ba2a66d98eb5-1107154/coakka-logger-mojo-0.1.0-source.tar.gz` | `0.1.0-source` | `0.1.0+ba2a66d98eb5` |
| Zig logger source connector | `logger/zig/releases/0.1.0+ba2a66d98eb5-1107154/coakka-logger-zig-0.1.0-source.tar.gz` | `0.1.0-source` | `0.1.0+ba2a66d98eb5` |

This refresh does not introduce a new native logger package generation. It
publishes the existing source-first Mojo/Zig logger lanes as evaluation
artifacts so the logger family no longer stops at JVM, Python, Node.js, Go,
C#, and Rust.

User-facing changes:

- Mojo logger now publishes a source package with bundled native logger
  libraries and a `CONSUMING.md`.
- Zig logger now publishes a source package with bundled native logger
  libraries and a `CONSUMING.md`.
- both source-package smokes now prefer packaged native libraries first, then
  fall back to repo-local staging only during source checkout work

Validation covered:

- Mojo logger source-owner smoke from source checkout
- Zig logger source-owner smoke from source checkout
- Mojo extracted-package smoke from the generated source tarball
- Zig extracted-package smoke from the generated source tarball
- public artifact surface verification in `coakka-publish`
