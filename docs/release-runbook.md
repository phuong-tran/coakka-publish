# CoAkka Release Runbook

This runbook is the release-order contract for CoAkka. Use it before touching a
registry, uploading a GitHub Release artifact, or updating public samples.

The artifact checklist in [release-checklist.md](release-checklist.md) remains
the low-level `coakka-publish` intake checklist. This runbook decides whether a
release lane is allowed to start, what must be finished first, and when it can
be merged.

## Release Rule

A release is not a package upload. A release is complete only when these
surfaces agree:

- source repository state
- `coakka-publish` public artifact and registry documentation
- package-manager registry, when the lane uses one
- `coakka-samples` pins and smoke paths
- Docker image tags, when samples or demos publish images
- release notes and compatibility matrix

Do not start from the package manager. Start from the compatibility anchor.

## Stop Conditions

Stop the release immediately when any of these are true:

- the native runtime or logger generation is not known
- language packages consume different native generations without an explicit
  compatibility note
- a package tree contains stale `.so`, `.dylib`, `.dll`, or archive payloads
  from another generation
- public package metadata leaks implementation details such as protobuf schemas,
  FFI/native bridge internals, CAF/libuv details, private paths, or repo-local
  links that become broken registry links
- samples consume a version that has not been registry-verified
- a registry upload failed and the registry has not been checked before retry
- the work mixes multiple release lanes without a written release state table

Fix the stop condition first. Do not compensate by updating docs around a bad
artifact.

## Release State Table

Create a small release state table before opening branches:

| Field | Value |
| --- | --- |
| Product lane | runtime, logger, runtime-client, runtime-inspect, adapter, or sample image |
| Target lane | Python, npm, JVM, Go, C#, Rust, Docker image, etc. |
| Compatibility anchor | native runtime or logger generation, for example `1.3.2+caff6d6d` |
| Source checkpoint | source repo commit that produced the package |
| Public artifact coordinate | GitHub Release path, Maven coordinate, npm package, PyPI package, etc. |
| Package-manager version | registry version, if any |
| Repos touched | source repo, `coakka-publish`, `coakka-samples`, core repo |
| Required smoke | exact commands that prove the public path works |
| Merge state | branch, PR, or main commit per repo |

If this table cannot be filled, the release is not ready to start.

## Phase 1: Compatibility Anchor

First decide whether the release changes the runtime or logger native
generation.

If the compatibility anchor changes:

1. Release or stage the native core first.
2. Retarget every dependent lane before publishing a single package-manager
   package:
   - JVM runtime and framework adapters
   - Python runtime
   - Node.js, Bun, and Electron runtime packages
   - Go, C#, Rust, Mojo, Zig, and Tauri lanes
   - `coakka-client`
   - `coakka-runtime-inspect`
   - Docker images and demo bundles
   - public samples that consume any of the above
3. Verify there are no stale platform binaries from the previous generation.
4. Update the compatibility matrix only after the full train is represented.

If the compatibility anchor does not change:

1. State that the release is a metadata, onboarding, or package-manager patch.
2. Keep the native generation unchanged in every public note.
3. Only update the target package and the docs/samples that consume that
   package-manager coordinate.

## Phase 2: Branch Setup

Use one release branch name across every repo involved in the lane, for example:

```text
release/python-pypi-onboarding
release/runtime-1.3.2-caff6d6d
```

Open the branch in every repo that will receive a commit:

- `coakkaCoreNativeDev` for native runtime/tooling changes
- `coakkaJVMConnector` for language/package source changes
- `coakka-publish` for public artifact and registry documentation
- `coakka-samples` for public sample pins, smoke paths, and Docker sample docs

Do not leave unrelated lane work on the same branch. If the scope changes,
record it in the release state table before continuing.

## Phase 3: Source Lane

Finish the source lane before touching public docs.

For each package source:

- bump the package version only after confirming the previous version cannot be
  reused
- set the native generation metadata explicitly
- remove stale native binaries from prior generations
- keep normal installs self-contained; do not require users to install native
  implementation dependencies
- preserve public API compatibility unless the release note declares a breaking
  change
- run the source repo readiness gate

Package metadata must pass an onboarding audit:

- registry README starts with a user-facing "New To CoAkka" or equivalent
  orientation
- public docs links are absolute URLs to `coakka-samples` or `coakka-publish`
- package-local docs links are valid in the registry renderer, or omitted
- no private filesystem paths appear in the package
- no implementation schemas or bridge internals leak into the user-facing
  package surface
- npm package metadata has repository, homepage, issues, license, keywords, and
  no `private=true`
- PyPI metadata has project URLs, license, classifiers, and no relative links
  that render under `pypi.org/project/<package>/...`

## Phase 4: Registry Publish

Registry versions are immutable. If metadata is wrong after upload, publish a
new patch version. Do not try to overwrite or reuse a version.

Before upload:

- run the package readiness gate
- run the package-manager metadata check, such as `twine check` for PyPI
- inspect the built package contents
- inspect the built package metadata text
- confirm the registry does not already contain the target version

After upload:

- verify the registry version exists through the registry API
- verify the rendered package page or metadata contains the intended
  onboarding text
- verify no broken registry-rendered links were introduced
- install from the registry in a clean environment
- run the smallest meaningful runtime smoke against the registry package
- revoke or rotate any token pasted into a chat or shell history

Only after these checks pass may `coakka-publish` and `coakka-samples` move to
the new package-manager coordinate.

## Phase 5: Public Artifact Repository

Update `coakka-publish` after the source lane or registry lane is verified.

Required updates:

- root README current lane summary
- [compatibility-matrix.md](compatibility-matrix.md)
- [package-manager-roadmap.md](package-manager-roadmap.md), when a package
  manager is involved
- release note under `docs/releases/`
- release-local `manifest.json` and `SHA256SUMS`, when adding or replacing
  GitHub Release artifacts
- `artifacts/public-artifacts.tsv`, when consumer-facing GitHub Release assets
  change

Run:

```sh
scripts/verify-public-surface.sh
```

For npm candidate sets, also run the npm candidate verifier. For JVM or Maven
artifacts, run the JVM native bundle gate from the artifact checklist.

## Phase 6: Public Samples

Update `coakka-samples` only after the public artifact or registry coordinate
exists and has been verified.

Required updates:

- install pins
- sample README text
- Docker image references, when image tags changed
- scripts that resolve public artifacts
- sample metadata helpers

Run:

```sh
bash scripts/check-artifact-pins.sh
```

Then run the smallest representative sample set for the lane. For a Python
runtime package-manager release, run:

```sh
bash run.sh runtime python basic
bash run.sh runtime python deadletter
bash run.sh runtime python hot-reload
bash runtime/scenarios/customer-crud/python-desktop-local/run.sh check
```

For npm runtime releases, run the matching Node, Bun, Electron, and
customer-scenario smoke paths that consume the registry packages.

## Phase 7: Merge Discipline

Finish one repo at a time:

1. Commit the verified source lane.
2. Push the branch.
3. Merge or open the PR for that repo.
4. Do not call the repo done until its branch is clean and its required checks
   have passed.
5. Move to the next repo.

The normal order is:

1. source repo
2. `coakka-publish`
3. `coakka-samples`
4. core repo follow-up docs, if needed

If a source change only prepares a future upload, do not update
`coakka-publish` or `coakka-samples` to the future version until the registry
upload succeeds.

## Python PyPI Lane Example

Use this order for a Python runtime package release:

1. Confirm the runtime native generation, for example `1.3.2+caff6d6d`.
2. Audit Python package native resources for all supported platforms:
   `macos-aarch64`, `linux-aarch64`, `linux-x86_64`, `windows-aarch64`, and
   `windows-x86_64`.
3. Confirm no stale `1.3.1` or previous-generation `.dll`, `.so`, or `.dylib`
   remains in the Python package tree.
4. Bump the PyPI package version.
5. Run:

```sh
./python/scripts/check_pypi_readiness.sh
python3 -m twine check python/build/wheelhouse/coakka_v2_connector-<version>-py3-none-any.whl
```

6. Inspect wheel metadata for:
   - correct version
   - correct native generation
   - `New To CoAkka` onboarding
   - no relative links to package-local docs
   - no private paths
7. Upload to PyPI.
8. Verify:

```sh
curl -fsS https://pypi.org/pypi/coakka-v2-connector/<version>/json
python3 -m venv /tmp/coakka-pypi-smoke
/tmp/coakka-pypi-smoke/bin/python -m pip install --no-cache-dir coakka-v2-connector==<version>
```

9. Run a clean import/version check from that venv.
10. Update `coakka-publish` to mark the new PyPI coordinate current.
11. Update `coakka-samples` pins to the new PyPI coordinate.
12. Run the Python sample smoke commands.
13. Commit, push, and merge each repo after its checks pass.

## Completion Checklist

The release is complete only when every row below is true:

| Check | Done |
| --- | --- |
| Compatibility anchor is documented | |
| All consuming lanes use the same anchor or have an explicit exception | |
| Source package version and native metadata are correct | |
| Package metadata/onboarding was inspected before upload | |
| Registry upload succeeded, if applicable | |
| Registry install smoke passed, if applicable | |
| `coakka-publish` docs and checks passed | |
| `coakka-samples` pins and smoke passed | |
| Docker images were updated and verified, if applicable | |
| Branches are merged or PRs are open with clear remaining work | |
| Tokens used during release were revoked or rotated | |
