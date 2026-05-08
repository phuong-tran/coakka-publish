#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
default_scanner="${repo_root}/scripts/scan-public-surface.sh"

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

file_digest() {
  local algorithm="$1"
  local path="$2"
  case "${algorithm}" in
    sha256)
      shasum -a 256 "${path}" | awk '{print $1}'
      ;;
    sha1)
      shasum -a 1 "${path}" | awk '{print $1}'
      ;;
    md5)
      if command -v md5sum >/dev/null 2>&1; then
        md5sum "${path}" | awk '{print $1}'
      elif command -v md5 >/dev/null 2>&1; then
        md5 -q "${path}"
      else
        echo "[verify-public-surface] md5sum or md5 is required for Maven md5 sidecars" >&2
        exit 69
      fi
      ;;
    *)
      echo "[verify-public-surface] unsupported digest algorithm: ${algorithm}" >&2
      exit 70
      ;;
  esac
}

verify_digest_sidecar() {
  local algorithm="$1"
  local file="$2"
  local sidecar="${file}.${algorithm}"
  local expected actual
  [[ -f "${sidecar}" ]] || return 0
  expected="$(awk '{print $1}' "${sidecar}")"
  actual="$(file_digest "${algorithm}" "${file}")"
  if [[ "${actual}" != "${expected}" ]]; then
    echo "[verify-public-surface] ${algorithm} mismatch for ${file#${repo_root}/}" >&2
    exit 1
  fi
}

verify_maven_sidecars() {
  local file
  while IFS= read -r -d '' file; do
    case "${file}" in
      *.md5|*.sha1|*.sha256) continue ;;
    esac
    verify_digest_sidecar sha256 "${file}"
    verify_digest_sidecar sha1 "${file}"
    verify_digest_sidecar md5 "${file}"
  done < <(find "${repo_root}/maven" -type f -print0)
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

if [[ ! -f "${repo_root}/runtime/native/releases/0.1.0+63c346e/SHA256SUMS" ]]; then
  echo "[verify-public-surface] missing runtime native release checksum file" >&2
  exit 1
fi

while IFS= read -r -d '' sums_file; do
  release_dir="$(dirname "${sums_file}")"
  verify_sha256_file "${release_dir#${repo_root}/}"
done < <(find "${repo_root}/logger" "${repo_root}/runtime/native" -path '*/releases/*/SHA256SUMS' -print0)

if find "${repo_root}/runtime" -mindepth 1 -maxdepth 1 -type d ! -name native -print -quit | grep -q .; then
  echo "[verify-public-surface] paused runtime language package lanes are present" >&2
  exit 1
fi

if find "${repo_root}/maven" -path '*/coakka/runtime/*' -print -quit | grep -q .; then
  echo "[verify-public-surface] paused runtime Maven artifacts are present" >&2
  exit 1
fi

verify_maven_sidecars

if [[ -n "${COAKKA_PUBLIC_SURFACE_SCANNER:-}" ]]; then
  "${COAKKA_PUBLIC_SURFACE_SCANNER}" "${repo_root}"
elif [[ -x "${default_scanner}" ]]; then
  "${default_scanner}" "${repo_root}"
else
  echo "[verify-public-surface] no public surface scanner found; skipped content scan" >&2
fi

echo "[verify-public-surface] ok"
