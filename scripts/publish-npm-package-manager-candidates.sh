#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
manifest="${repo_root}/package-manager/npm/candidates/b46f705/manifest.json"
mode=""
tag="latest"
otp=""

usage() {
  cat >&2 <<'EOF'
Usage:
  scripts/publish-npm-package-manager-candidates.sh --dry-run
  scripts/publish-npm-package-manager-candidates.sh --publish

Options:
  --manifest PATH   Candidate manifest to publish.
  --tag TAG         npm dist-tag, defaults to latest.
  --otp CODE       npm one-time password for accounts with publish 2FA.

The script always verifies candidate checksums and package-manager metadata
before invoking npm publish.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --dry-run|--publish)
      if [[ -n "${mode}" ]]; then
        usage
        exit 2
      fi
      mode="$1"
      shift
      ;;
    --manifest)
      manifest="$2"
      shift 2
      ;;
    --tag)
      tag="$2"
      shift 2
      ;;
    --otp)
      otp="$2"
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

if [[ -z "${mode}" ]]; then
  usage
  exit 2
fi

candidate_dir="$(cd "$(dirname "${manifest}")" && pwd)"

"${repo_root}/scripts/test-npm-package-manager-candidates.sh" --require-public-metadata "${manifest}"

if [[ "${mode}" == "--publish" ]]; then
  npm whoami >/dev/null
fi

python3 - "${manifest}" <<'PY' |
import json
import sys

manifest_path = sys.argv[1]
with open(manifest_path, "r", encoding="utf-8") as fh:
    manifest = json.load(fh)

packages = {entry["label"]: entry for entry in manifest["packages"]}
for label in manifest.get("publish_order", []):
    entry = packages[label]
    print(f"{label}\t{entry['package_name']}\t{entry['relative_path']}")
PY
while IFS=$'\t' read -r label package_name relative_path; do
  artifact="${candidate_dir}/${relative_path}"
  echo "[npm-publish-candidate] ${label}: ${package_name}"
  if [[ "${mode}" == "--publish" ]] &&
      "${repo_root}/scripts/verify-npm-registry-candidates.sh" \
        --manifest "${manifest}" \
        --only "${label}" >/dev/null 2>&1; then
    echo "[npm-publish-candidate] ${label}: already published and verified"
    continue
  fi
  publish_args=(--tag "${tag}")
  if [[ -n "${otp}" ]]; then
    publish_args+=(--otp "${otp}")
  fi
  if [[ "${mode}" == "--dry-run" ]]; then
    npm publish --dry-run "${publish_args[@]}" "${artifact}"
  else
    npm publish "${publish_args[@]}" "${artifact}"
  fi
done

echo "[npm-publish-candidate] ok"
