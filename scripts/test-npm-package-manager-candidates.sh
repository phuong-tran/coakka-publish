#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
manifest="${1:-${repo_root}/package-manager/npm/candidates/f3e25ec/manifest.json}"
candidate_dir="$(cd "$(dirname "${manifest}")" && pwd)"

(
  cd "${candidate_dir}"
  shasum -a 256 -c SHA256SUMS >/dev/null
)

python3 "${repo_root}/scripts/verify-npm-package-manager-artifact.py" \
  --candidate-manifest "${manifest}"

echo "[npm-package-manager-candidates-test] ok"
