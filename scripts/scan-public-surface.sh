#!/usr/bin/env bash

set -euo pipefail

if [[ "$#" -lt 1 ]]; then
  echo "usage: $0 <path> [<path>...]" >&2
  exit 64
fi

if ! command -v strings >/dev/null 2>&1; then
  echo "[public-artifact-surface] strings is required" >&2
  exit 69
fi

blocked_re="${COAKKA_PUBLIC_FORBIDDEN_RE:-(^|[^[:alnum:]_])(CAF|caf)([^[:alnum:]_]|$)|libcaf|caf_|_caf|CAF_|_CAF|caf-|CAF-|(^|[^[:alnum:]_])libuv([^[:alnum:]_]|$)|libuv_|_libuv|southbound|Southbound|SOUTHBOUND|remote_wire|REMOTE_WIRE|wire_profile|WIRE_PROFILE|COAKKA_V2_ENABLE_CAF_BACKEND|COAKKA_V2_RUNTIME_FEATURE_CAF_BACKEND|coakkaCore|external/caf|external/libuv}"
self_path="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "$0")"
tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-surface.XXXXXX")"
violations_file="${tmp_root}/violations.txt"
extract_counter=0

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

extract_archive() {
  local archive="$1"
  local dest="$2"
  case "${archive}" in
    *.tar.gz|*.tgz)
      mkdir -p "${dest}"
      tar -xzf "${archive}" -C "${dest}"
      ;;
    *.tar)
      mkdir -p "${dest}"
      tar -xf "${archive}" -C "${dest}"
      ;;
    *.zip|*.jar|*.whl|*.nupkg)
      if ! command -v unzip >/dev/null 2>&1; then
        echo "[public-artifact-surface] unzip is required for ${archive}" >&2
        exit 69
      fi
      mkdir -p "${dest}"
      unzip -qq "${archive}" -d "${dest}"
      ;;
    *)
      return 1
      ;;
  esac
}

scan_file() {
  local file="$1"
  local label="$2"
  local matches

  if [[ "${file}" == "${self_path}" ]]; then
    return 0
  fi

  matches="$(strings -a "${file}" 2>/dev/null | grep -En "${blocked_re}" | head -n 20 || true)"
  if [[ -n "${matches}" ]]; then
    {
      echo "[public-artifact-surface] forbidden marker in ${label}"
      echo "${matches}"
    } >>"${violations_file}"
  fi
}

scan_tree() {
  local root="$1"
  local label_prefix="$2"
  local file rel extract_dir

  while IFS= read -r -d '' file; do
    rel="${file#${root}/}"
    scan_file "${file}" "${label_prefix}/${rel}"
    extract_dir="${tmp_root}/nested-${extract_counter}"
    extract_counter=$((extract_counter + 1))
    if extract_archive "${file}" "${extract_dir}"; then
      scan_tree "${extract_dir}" "${label_prefix}/${rel}"
    else
      rm -rf "${extract_dir}"
    fi
  done < <(find "${root}" -type f ! -path '*/.git/*' -print0)
}

for input in "$@"; do
  if [[ ! -e "${input}" ]]; then
    echo "[public-artifact-surface] missing path: ${input}" >&2
    exit 66
  fi

  if [[ -d "${input}" ]]; then
    scan_tree "${input}" "${input}"
  else
    scan_file "${input}" "${input}"
    extract_dir="${tmp_root}/top-${extract_counter}"
    extract_counter=$((extract_counter + 1))
    if extract_archive "${input}" "${extract_dir}"; then
      scan_tree "${extract_dir}" "${input}"
    else
      rm -rf "${extract_dir}"
    fi
  fi
done

if [[ -s "${violations_file}" ]]; then
  cat "${violations_file}" >&2
  exit 1
fi

echo "[public-artifact-surface] ok"
