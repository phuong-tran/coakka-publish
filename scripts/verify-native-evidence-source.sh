#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
source_dir="${repo_root}/runtime-test"
manifest="${source_dir}/SOURCE-MANIFEST.sha256"

if [[ ! -f "${manifest}" ]]; then
  printf 'native evidence source manifest is missing: %s\n' "${manifest}" >&2
  exit 1
fi

if command -v sha256sum >/dev/null 2>&1; then
  (cd "${source_dir}" && sha256sum --check "${manifest}")
elif command -v shasum >/dev/null 2>&1; then
  (cd "${source_dir}" && shasum -a 256 --check "${manifest}")
else
  printf 'sha256sum or shasum is required\n' >&2
  exit 1
fi

printf 'native evidence source manifest verified\n'
