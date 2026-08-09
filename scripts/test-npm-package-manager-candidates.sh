#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
require_public_metadata="false"
manifest="${repo_root}/package-manager/npm/candidates/4782dcd/manifest.json"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --require-public-metadata)
      require_public_metadata="true"
      shift
      ;;
    *)
      manifest="$1"
      shift
      ;;
  esac
done

candidate_dir="$(cd "$(dirname "${manifest}")" && pwd)"

(
  cd "${candidate_dir}"
  shasum -a 256 -c SHA256SUMS >/dev/null
)

verify_args=(--candidate-manifest "${manifest}")
if [[ "${require_public_metadata}" == "true" ]]; then
  verify_args+=(--require-public-metadata)
fi

python3 "${repo_root}/scripts/verify-npm-package-manager-artifact.py" "${verify_args[@]}"

echo "[npm-package-manager-candidates-test] ok"
