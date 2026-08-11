# Contributing To coakka-publish

Thanks for contributing.

This repository is the public artifact and release-metadata surface for
CoAkka. It is not the source-build home for runtime-core, connectors, or
private addon work.

## What This Repo Accepts

Good contributions here include:

- release-note fixes
- manifest or checksum corrections
- public artifact index fixes
- documentation clarifications about published artifact scope
- verifier improvements for the public artifact surface
- runtime-addon release intake metadata produced by its source-owner repository

## What Should Go Somewhere Else

Open or route changes elsewhere when the work is really about:

- runtime-core source code
- connector implementation code
- sample applications and public walkthroughs
- private addon incubation
- source-build orchestration

Those changes belong in their source-owner repositories.

## Public Artifact Rule

Keep this repository an artifact repository.

- do not turn it into a source-build workspace
- do not add unpublished internal implementation details unless the public
  artifact contract truly requires them
- keep release directories, manifests, and checksums aligned
- keep current release notes distinct from historical ones

If a lane is refreshed, the corresponding public metadata should move together:

- release README
- `manifest.json`
- `SHA256SUMS`
- public artifact index rows
- release note

Optional runtime addons use the same rule under
`runtime-addons/<addon>/native/releases/`. Addon implementation work remains in
its source-owner repository; this repository accepts only release-shaped
archives, manifests, checksums, compatibility evidence, and public docs.

## Version And Compatibility Rule

Do not publish mixed artifact stories by accident.

If a runtime or logger generation changes, make the compatibility statement
explicit and keep the affected language or framework lanes aligned with that
truth.

If a release note declares a compatibility exception, say so directly.
Otherwise, keep the public surface consistent.

## Before Opening A PR

Please run the relevant verifier path for your change.

Common checks:

```sh
scripts/verify-public-surface.sh
scripts/test-public-artifact-manifest.sh
scripts/test-runtime-addon-release.sh
```

If the change touches the JVM runtime bundle or Maven lane, also run:

```sh
scripts/test-runtime-jvm-native-bundle.sh
```

## PR Notes

Keep PRs narrow and release-shaped.

Please say:

- which lane changed
- which version or generation changed
- which verifiers you ran
- whether another repository must land first or at the same time

If a change depends on source-owner output that is not published yet, call that
out clearly instead of making this repo guess.
