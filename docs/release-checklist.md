# Public Release Checklist

This checklist is for refreshing the public artifact surface in this repository.
It keeps the public story aligned without turning `coakka-publish` into a
source-build workspace.

## Scope

Before copying artifacts into this repository, identify:

- product lane: runtime, logger, runtime-client, runtime-inspect, framework
  adapter, or sample image metadata
- public version and release directory
- source checkpoint used to produce the artifact
- native runtime or logger native generation consumed by the artifact
- target platforms covered by the artifact
- whether the lane is a binary package, Maven artifact, native archive, Docker
  bundle, or source connector package
- whether this release also opens or updates a package-manager channel

Do not publish a lane if its public artifact generation and compatibility
anchor are unclear.

## Artifact Intake

For each public artifact:

- place it under the correct product and language lane
- keep `manifest.json` beside the artifact when the lane uses release
  manifests
- refresh `SHA256SUMS` in the release directory
- add or update the row in `artifacts/public-artifacts.tsv` for
  consumer-facing downloads
- keep Maven checksum sidecars current for Maven artifacts
- ensure the package does not require users to install native implementation
  dependencies outside the artifact contract
- keep package-manager metadata aligned with the same generation when a
  package-manager lane exists

For runtime connector packages, run the intake gate before copying the artifact
into its release directory:

```bash
scripts/verify-runtime-intake-artifact.py \
  --lane node \
  --artifact /path/to/artifact.tgz \
  --expected-native-version 1.3.1+bda2ef5
```

Use the matching `--lane` value and expected native generation for the release.
Main runtime connector packages must include or resolve the expected runtime
native library for the target platform.

## Documentation

For each release train update:

- update the root README current status if public generations changed
- update [compatibility-matrix.md](compatibility-matrix.md) when a current lane
  generation changes
- add or update the release note under `docs/releases/`
- update [../CHANGELOG.md](../CHANGELOG.md) with the user-visible change
- keep release notes specific to public behavior, artifact layout, checksums,
  and compatibility
- do not document a connector helper as released until the published artifact
  contains it
- update [package-manager-roadmap.md](package-manager-roadmap.md) if a planned
  npm, Go module, crates.io, apt/deb, or other package-manager lane changes
  status

Release docs should say what a consumer can download and run. They should not
turn source-owner implementation details into the public contract.

## Verification

Run the public surface workflow locally before pushing:

```bash
bash -n \
  scripts/scan-public-surface.sh \
  scripts/test-public-surface-scanner.sh \
  scripts/test-public-artifact-manifest.sh \
  scripts/test-npm-package-manager-artifact.sh \
  scripts/test-npm-package-manager-candidates.sh \
  scripts/test-runtime-intake-artifact.sh \
  scripts/verify-public-surface.sh
python3 -m py_compile scripts/verify-runtime-intake-artifact.py
python3 -m py_compile scripts/verify-npm-package-manager-artifact.py
scripts/test-public-surface-scanner.sh
scripts/test-public-artifact-manifest.sh
scripts/test-npm-package-manager-artifact.sh
scripts/test-npm-package-manager-candidates.sh
scripts/test-runtime-intake-artifact.sh
scripts/verify-public-surface.sh
```

If the change touches the JVM runtime bundle or Maven lane, also run:

```bash
scripts/test-runtime-jvm-native-bundle.sh
```

If the change introduces a new package lane, add or update focused fixture
coverage for the manifest, intake, or public boundary gate before publishing.

If the change introduces a package-manager lane, also verify the package
metadata or repository metadata that users will consume. Do not mark the lane
current in docs until CI covers that public path.

For staged npm candidate tarballs, run:

```bash
scripts/test-npm-package-manager-candidates.sh
scripts/publish-npm-package-manager-candidates.sh --dry-run
```

After publishing staged npm candidates with real credentials, run:

```bash
scripts/publish-npm-package-manager-candidates.sh --publish --otp <current-code>
scripts/verify-npm-registry-candidates.sh
```

For current public npm candidates, run the package-manager preflight before
moving a lane from planned to current:

```bash
scripts/verify-npm-package-manager-artifact.py --current-candidates
```

That preflight is stricter than the GitHub Release artifact gate: it requires
registry-ready metadata, no `private=true`, explicit license carry-through, no
install-time native setup scripts, no raw/file/git dependencies, no `.proto`
schema leakage, and native delivery through the package itself or through the
matching first-party Node package for Electron.

## GitHub Release

When updating the GitHub Release page:

- upload all current consumer-facing artifacts for the release train
- upload the current `artifacts/public-artifacts.tsv`
- verify the release asset count and manifest rows agree
- update the release body with the current public surface summary
- keep raw GitHub artifact paths and release assets aligned

After uploading, use a clean checkout or the public sample repository to verify
that artifacts resolve from the public surface, not from a private build tree.

## Post-Release

After the publish repo is green:

- update `coakka-samples` pins and sample docs if public paths changed
- run the relevant `coakka-samples` artifact pin checks and sample smoke
- update sample walkthrough docs or recordings only when the runnable sample
  behavior changed
- make sure the public release note, compatibility matrix, changelog, and
  sample surface all describe the same generation

The release is not finished until both the artifact repository and public
sample repository agree on the public surface.
