# Security Policy

## Scope

This repository contains the public artifact surface for CoAkka:

- release manifests
- checksums
- release notes
- public artifact indexes
- published binary and package layout

If you believe you found a security issue in the published artifact surface or
release metadata, report it privately first.

## How To Report

Please do not open a public issue first for a suspected security problem.

Instead:

- use the repository security reporting flow if it is enabled
- or contact the repository owner privately and clearly mark the report as a
  security issue
- or contact `gabrielgun1983@gmail.com` directly and clearly mark the report as
  a security issue

General support contact is listed in [SUPPORT.md](../SUPPORT.md).

## What To Include

Please include:

- affected artifact lane
- affected version or generation
- affected file, manifest, or checksum path
- reproduction or verification steps
- whether the issue appears limited to publish metadata or may affect the
  source-owner build outputs too

## Coordination Note

Some issues reported here may need coordinated fixes across:

- `coakka-publish`
- source-owner repositories
- public sample repositories

Public disclosure may wait until the corresponding release-train fix is ready.
