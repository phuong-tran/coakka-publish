#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ "$#" -lt 1 ]]; then
  echo "usage: $0 <path> [<path>...]" >&2
  exit 64
fi

if ! command -v strings >/dev/null 2>&1; then
  echo "[public-artifact-surface] strings is required" >&2
  exit 69
fi

default_blocked_re='/(Users|home)/[^[:space:]/]+/(misc|study|workspace|src|dev)/'
blocked_re="${COAKKA_PUBLIC_FORBIDDEN_RE:-${default_blocked_re}}"
self_path="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "$0")"
tmp_parent="${COAKKA_PUBLIC_TMP_ROOT:-${repo_root}/.tmp}"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/coakka-public-surface.XXXXXX")"
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
      COPYFILE_DISABLE=1 tar -xzf "${archive}" -C "${dest}"
      ;;
    *.tar)
      mkdir -p "${dest}"
      COPYFILE_DISABLE=1 tar -xf "${archive}" -C "${dest}"
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

  if [[ "${file}" == "${self_path}" || "$(cd "$(dirname "${file}")" && pwd)/$(basename "${file}")" == "${self_path}" ]]; then
    return 0
  fi

  matches="$(strings -a "${file}" 2>/dev/null | grep -En "${blocked_re}" | head -n 20 || true)"
  if [[ -n "${matches}" ]]; then
    {
      echo "[public-artifact-surface] forbidden marker in ${label}"
    } >>"${violations_file}"
  fi
}

scan_tree() {
  local root="$1"
  local label_prefix="$2"
  local file rel extract_dir

  while IFS= read -r -d '' file; do
    rel="${file#"${root}"/}"
    extract_dir="${tmp_root}/nested-${extract_counter}"
    extract_counter=$((extract_counter + 1))
    if extract_archive "${file}" "${extract_dir}"; then
      scan_tree "${extract_dir}" "${label_prefix}/${rel}"
    else
      rm -rf "${extract_dir}"
      scan_file "${file}" "${label_prefix}/${rel}"
    fi
  done < <(
    if [[ "${root}" == "${tmp_parent}" || "${root}" == "${tmp_parent}/"* ]]; then
      find "${root}" \
        -path '*/.git/*' -prune -o \
        -path '*/.github/*' -prune -o \
        -path '*/.idea/*' -prune -o \
        -type f -print0
    else
      find "${root}" \
        -path "${tmp_parent}" -prune -o \
        -path "${tmp_parent}/*" -prune -o \
        -path '*/.git/*' -prune -o \
        -path '*/.github/*' -prune -o \
        -path '*/.idea/*' -prune -o \
        -type f -print0
    fi
  )
}

for input in "$@"; do
  if [[ ! -e "${input}" ]]; then
    echo "[public-artifact-surface] missing path: ${input}" >&2
    exit 66
  fi

  if [[ -d "${input}" ]]; then
    scan_tree "${input}" "${input}"
  else
    extract_dir="${tmp_root}/top-${extract_counter}"
    extract_counter=$((extract_counter + 1))
    if extract_archive "${input}" "${extract_dir}"; then
      scan_tree "${extract_dir}" "${input}"
    else
      rm -rf "${extract_dir}"
      scan_file "${input}" "${input}"
    fi
  fi
done

if [[ -s "${violations_file}" ]]; then
  cat "${violations_file}" >&2
  exit 1
fi

echo "[public-artifact-surface] ok"
