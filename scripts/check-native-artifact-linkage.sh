#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
manifest="${repo_root}/artifacts/public-artifacts.tsv"
tmp_root="${COAKKA_NATIVE_LINKAGE_TMP_ROOT:-${repo_root}/.tmp/native-linkage}"

fail() {
  echo "[native-artifact-linkage] $*" >&2
  exit 1
}

require_command() {
  command -v "$1" >/dev/null 2>&1 || fail "$1 is required"
}

dependency_allowed() {
  local dep lower
  dep="$1"
  lower="$(printf '%s' "${dep}" | tr 'A-Z' 'a-z')"

  case "${lower}" in
    libcoakka_runtime_v2.so|libcoakka_runtime_v2.dll|libcoakka_logger_core.so|libcoakka_logger_core.so.10|libcoakka_logger_core.dll)
      return 0
      ;;
    libm.so.6|libc.so.6|ld-linux-x86-64.so.2|ld-linux-aarch64.so.1)
      return 0
      ;;
    kernel32.dll|ntdll.dll|ws2_32.dll|advapi32.dll|user32.dll|iphlapi.dll|iphlpapi.dll|userenv.dll|shell32.dll|ole32.dll|dbghelp.dll)
      return 0
      ;;
    api-ms-win-crt-*.dll)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

check_dependencies_for_file() {
  local file="$1"
  local label="$2"
  local report dep

  report="$(objdump -p "${file}" 2>/dev/null || true)"
  [[ -n "${report}" ]] || return 0

  while IFS= read -r dep; do
    [[ -n "${dep}" ]] || continue
    dependency_allowed "${dep}" ||
      fail "artifact declares a non-allowed dynamic dependency: ${label}"
  done < <(
    printf '%s\n' "${report}" |
      awk '
        $1 == "NEEDED" { print $2 }
        $1 == "DLL" && $2 == "Name:" { print $3 }
      '
  )
}

check_tree() {
  local root="$1"
  local label_prefix="$2"
  local file rel

  while IFS= read -r -d '' file; do
    rel="${file#"${root}"/}"
    check_dependencies_for_file "${file}" "${label_prefix}/${rel}"
  done < <(find "${root}" -type f -print0)
}

check_archive() {
  local relative_path="$1"
  local archive="${repo_root}/${relative_path}"
  local dest

  [[ -f "${archive}" ]] || fail "missing artifact: ${relative_path}"
  dest="$(mktemp -d "${tmp_root}/archive.XXXXXX")"
  COPYFILE_DISABLE=1 tar -xzf "${archive}" -C "${dest}"
  check_tree "${dest}" "${relative_path}"
}

require_command objdump
require_command tar
require_command awk

[[ -f "${manifest}" ]] || fail "missing manifest: artifacts/public-artifacts.tsv"
mkdir -p "${tmp_root}"
trap 'rm -rf "${tmp_root}"' EXIT

while IFS=$'\t' read -r row_status label relative_path expected_sha extra || [[ -n "${row_status:-}" ]]; do
  [[ -z "${row_status:-}" || "${row_status}" == \#* ]] && continue
  [[ "${row_status}" == "public" ]] || continue
  case "${relative_path}" in
    logger/native/releases/*.tar.gz|runtime/native/releases/*.tar.gz|runtime-inspect/native/releases/*.tar.gz|cli/releases/*.tar.gz|demo/coakka-client/releases/*.tar.gz|coakka-tools/coakka-client/releases/*/*.tar.gz|coakka-tools/coakka-client/docker-demo/releases/*/*.tar.gz|coakka-tools/coakka-runtime-inspect/releases/*/*.tar.gz)
      check_archive "${relative_path}"
      ;;
  esac
done <"${manifest}"

if [[ -d "${repo_root}/native" ]]; then
  check_tree "${repo_root}/native" "native"
fi

echo "[native-artifact-linkage] ok"
