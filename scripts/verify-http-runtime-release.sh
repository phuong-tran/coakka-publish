#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
release_root="${repo_root}/coakka-http-runtime"
native_release="1.0.0+df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431"
connector_release="1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-cec8e11"

fail() {
  echo "[http-runtime-release] $*" >&2
  exit 1
}

verify_single_release() {
  local lane="$1"
  local expected="$2"
  local releases="${release_root}/${lane}/releases"
  local actual=()

  [[ -d "${releases}/${expected}" ]] ||
    fail "missing ${lane} release ${expected}"
  while IFS= read -r directory; do
    actual+=("$(basename "${directory}")")
  done < <(find "${releases}" -mindepth 1 -maxdepth 1 -type d | sort)

  if [[ "${#actual[@]}" -ne 1 || "${actual[0]}" != "${expected}" ]]; then
    fail "${lane} release directory set is not exact: ${actual[*]:-empty}"
  fi

  (cd "${releases}/${expected}" && shasum -a 256 -c SHA256SUMS >/dev/null)
}

verify_single_release native "${native_release}"
for lane in jvm python javascript go; do
  verify_single_release "${lane}" "${connector_release}"
done

python3 - "${release_root}" "${native_release}" "${connector_release}" <<'PY'
import hashlib
import json
import os
import sys

release_root, native_release, connector_release = sys.argv[1:]
version = "1.0.0"
native_source = "df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431"
core_source = "3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0"
connector_source = "cec8e116ba09f76f215311167c866213194051ee"


def fail(message: str) -> None:
    print(f"[http-runtime-release] {message}", file=sys.stderr)
    raise SystemExit(1)


def read_manifest(lane: str, release: str) -> tuple[str, dict]:
    path = os.path.join(release_root, lane, "releases", release, "manifest.json")
    with open(path, "r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    if manifest.get("version") != version:
        fail(f"{lane} package version is not {version}")
    if manifest.get("release_directory") != release:
        fail(f"{lane} manifest release directory does not match its path")
    if manifest.get("status") != "released" or manifest.get("distribution") != "github":
        fail(f"{lane} distribution status is not the GitHub release contract")
    if manifest.get("registry_mirrors") != []:
        fail(f"{lane} unexpectedly declares a registry mirror")
    return os.path.dirname(path), manifest


def verify_file(root: str, item: dict) -> None:
    relative = item["file"]
    path = os.path.join(root, relative)
    if not os.path.isfile(path):
        fail(f"missing artifact {relative}")
    size = os.path.getsize(path)
    if "size" in item and item["size"] != size:
        fail(f"size mismatch for {relative}")
    with open(path, "rb") as handle:
        digest = hashlib.sha256(handle.read()).hexdigest()
    if item["sha256"] != digest:
        fail(f"SHA-256 mismatch for {relative}")


native_root, native = read_manifest("native", native_release)
if native.get("implementation_source_commit") != native_source:
    fail("native source identity does not match the release suffix")
if native_release != f"{version}+{native_source}":
    fail("native release suffix is malformed")
for target in native.get("targets", []):
    verify_file(native_root, {"file": target["library"], "sha256": target["sha256"]})
    if "import_library" in target:
        verify_file(
            native_root,
            {
                "file": target["import_library"],
                "sha256": target["import_library_sha256"],
            },
        )

if connector_release != f"{version}+{core_source}-{connector_source[:7]}":
    fail("connector release suffix is malformed")

for lane in ("jvm", "python", "javascript", "go"):
    root, manifest = read_manifest(lane, connector_release)
    recorded_source = manifest.get("connector_source_commit", manifest.get("source_commit"))
    if manifest.get("core_source_commit") != core_source or recorded_source != connector_source:
        fail(f"{lane} source identity does not match the release suffix")
    if lane == "jvm":
        items = manifest.get("targets", [])
    elif lane == "python":
        items = manifest.get("artifacts", [])
    else:
        items = [manifest.get("artifact", {})]
    if not items:
        fail(f"{lane} manifest has no artifacts")
    for item in items:
        verify_file(root, item)

print("[http-runtime-release] ok: version=1.0.0 lanes=5 immutable-identities=2")
PY
