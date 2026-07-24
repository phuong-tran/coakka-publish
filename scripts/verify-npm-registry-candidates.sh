#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
manifest="${repo_root}/package-manager/npm/candidates/55bbeb7/manifest.json"
tag="latest"

usage() {
  cat >&2 <<'EOF'
Usage:
  scripts/verify-npm-registry-candidates.sh [--manifest PATH] [--tag TAG]

Verifies that staged npm candidate tarballs have been published to the npm
registry with matching version, license, dependency metadata, shasum, and
dist-tag.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --manifest)
      manifest="$2"
      shift 2
      ;;
    --tag)
      tag="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      usage
      exit 2
      ;;
  esac
done

"${repo_root}/scripts/test-npm-package-manager-candidates.sh" "${manifest}"

python3 - "${manifest}" "${tag}" <<'PY'
import hashlib
import json
import subprocess
import sys
import tarfile
from pathlib import Path

manifest_path = Path(sys.argv[1]).resolve()
tag = sys.argv[2]
candidate_dir = manifest_path.parent


def fail(message: str) -> None:
    print(f"[npm-registry-candidates] {message}", file=sys.stderr)
    sys.exit(1)


def npm_view_json(spec: str, field: str | None = None) -> object:
    args = ["npm", "view", spec]
    if field:
        args.append(field)
    args.append("--json")
    result = subprocess.run(args, text=True, capture_output=True)
    if result.returncode != 0:
        detail = result.stderr.strip() or result.stdout.strip()
        raise RuntimeError(detail)
    text = result.stdout.strip()
    if not text:
        return {}
    return json.loads(text)


def read_local_package(artifact: Path) -> dict:
    with tarfile.open(artifact, "r:gz") as archive:
        try:
            member = archive.extractfile("package/package.json")
        except KeyError:
            fail(f"{artifact} is missing package/package.json")
        if member is None:
            fail(f"{artifact} package/package.json is not a regular file")
        return json.loads(member.read().decode("utf-8"))


def file_sha1(path: Path) -> str:
    digest = hashlib.sha1()
    with path.open("rb") as fh:
        for block in iter(lambda: fh.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


with manifest_path.open("r", encoding="utf-8") as fh:
    manifest = json.load(fh)

packages = {entry["label"]: entry for entry in manifest["packages"]}
labels = manifest.get("publish_order") or list(packages)
failures: list[str] = []

for label in labels:
    entry = packages[label]
    artifact = candidate_dir / entry["relative_path"]
    local = read_local_package(artifact)
    name = local["name"]
    version = local["version"]
    spec = f"{name}@{version}"
    print(f"[npm-registry-candidates] verifying {label}: {spec}")
    try:
        published = npm_view_json(spec)
        dist_tags = npm_view_json(name, "dist-tags")
    except RuntimeError as exc:
        failures.append(f"{spec}: {exc}")
        continue

    checks = {
        "version": version,
        "license": local.get("license"),
        "dependencies": local.get("dependencies") or {},
    }
    for key, expected in checks.items():
        actual = published.get(key)
        if key == "dependencies":
            actual = actual or {}
        if actual != expected:
            failures.append(f"{spec}: {key} mismatch: expected {expected!r}, found {actual!r}")

    expected_shasum = file_sha1(artifact)
    actual_shasum = (published.get("dist") or {}).get("shasum")
    if actual_shasum != expected_shasum:
        failures.append(f"{spec}: shasum mismatch: expected {expected_shasum}, found {actual_shasum}")

    if isinstance(dist_tags, dict) and dist_tags.get(tag) != version:
        failures.append(f"{name}: dist-tag {tag!r} points to {dist_tags.get(tag)!r}, expected {version!r}")

if failures:
    for failure in failures:
        print(f"[npm-registry-candidates] blocked: {failure}", file=sys.stderr)
    sys.exit(1)

print("[npm-registry-candidates] ok")
PY
