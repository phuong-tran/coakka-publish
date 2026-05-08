#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

require_file() {
  local path="$1"
  if [[ ! -f "${repo_root}/${path}" ]]; then
    echo "[verify-public-surface] missing required file: ${path}" >&2
    exit 1
  fi
}

verify_sha256_file() {
  local dir="$1"
  local sums="${repo_root}/${dir}/SHA256SUMS"
  if [[ ! -f "${sums}" ]]; then
    echo "[verify-public-surface] missing checksum file: ${dir}/SHA256SUMS" >&2
    exit 1
  fi
  (cd "${repo_root}/${dir}" && shasum -a 256 -c SHA256SUMS >/dev/null)
}

require_file "README.md"
require_file "docs/public-artifact-contract.md"
require_file "include/coakka/v2/client.h"
require_file "include/coakka/v2/control.h"
require_file "include/coakka/v2/runtime.h"
require_file "include/coakka/v2/transport.h"
require_file "include/coakka/v2/utils.h"
require_file "native/linux-aarch64/libcoakka_runtime_v2.so"
require_file "native/linux-x86_64/libcoakka_runtime_v2.so"
require_file "native/macos-aarch64/libcoakka_runtime_v2.dylib"
require_file "SHA256SUMS"

(cd "${repo_root}" && shasum -a 256 -c SHA256SUMS >/dev/null)

while IFS= read -r -d '' sums_file; do
  release_dir="$(dirname "${sums_file}")"
  verify_sha256_file "${release_dir#${repo_root}/}"
done < <(find "${repo_root}/logger" -path '*/releases/*/SHA256SUMS' -print0)

if [[ -f "${repo_root}/runtime/native/releases/0.1.0+63c346e/SHA256SUMS" ]]; then
  verify_sha256_file "runtime/native/releases/0.1.0+63c346e"
else
  echo "[verify-public-surface] missing runtime native release checksum file" >&2
  exit 1
fi

if find "${repo_root}/runtime" -mindepth 1 -maxdepth 1 -type d ! -name native -print -quit | grep -q .; then
  echo "[verify-public-surface] paused runtime language package lanes are present" >&2
  exit 1
fi

if find "${repo_root}/maven" -path '*/coakka/runtime/*' -print -quit | grep -q .; then
  echo "[verify-public-surface] paused runtime Maven artifacts are present" >&2
  exit 1
fi

if [[ -n "${COAKKA_PUBLIC_SURFACE_SCANNER:-}" ]]; then
  "${COAKKA_PUBLIC_SURFACE_SCANNER}" "${repo_root}"
else
  echo "[verify-public-surface] COAKKA_PUBLIC_SURFACE_SCANNER not set; skipped content scan" >&2
fi

echo "[verify-public-surface] ok"
