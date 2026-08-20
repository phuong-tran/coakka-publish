#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
manifest="${repo_root}/artifacts/public-artifacts.tsv"
tmp_root="${COAKKA_NATIVE_LINKAGE_TMP_ROOT:-}"

fail() {
  echo "[native-artifact-linkage] $*" >&2
  exit 1
}

require_command() {
  command -v "$1" >/dev/null 2>&1 || fail "$1 is required"
}

dependency_allowed() {
  local dep allowed_addon_dependencies lower allowed
  dep="$1"
  allowed_addon_dependencies="${2:-}"
  lower="$(printf '%s' "${dep}" | tr '[:upper:]' '[:lower:]')"

  case "${lower}" in
    libcoakka_runtime_v2.so|libcoakka_runtime_v2.dll|libcoakka_logger_core.so|libcoakka_logger_core.so.10|libcoakka_logger_core.dll)
      return 0
      ;;
    libm.so.6|libc.so.6|libstdc++.so.6|libgcc_s.so.1|libz.so.1|libzstd.so.1|ld-linux-x86-64.so.2|ld-linux-aarch64.so.1)
      return 0
      ;;
    kernel32.dll|ntdll.dll|ws2_32.dll|advapi32.dll|bcrypt.dll|crypt32.dll|secur32.dll|user32.dll|iphlapi.dll|iphlpapi.dll|userenv.dll|shell32.dll|ole32.dll|dbghelp.dll)
      return 0
      ;;
    msvcp140.dll|msvcp140_atomic_wait.dll|vcruntime140.dll|vcruntime140_1.dll)
      return 0
      ;;
    api-ms-win-crt-*.dll)
      return 0
      ;;
    *)
      while IFS= read -r allowed; do
        [[ -n "${allowed}" ]] || continue
        if [[ "${lower}" == "$(printf '%s' "${allowed}" | tr '[:upper:]' '[:lower:]')" ]]; then
          return 0
        fi
      done <<<"${allowed_addon_dependencies}"
      return 1
      ;;
  esac
}

check_dependencies_for_file() {
  local file="$1"
  local label="$2"
  local allowed_addon_dependencies="${3:-}"
  local report dep

  report="$(objdump -p "${file}" 2>/dev/null || true)"
  [[ -n "${report}" ]] || return 0

  while IFS= read -r dep; do
    [[ -n "${dep}" ]] || continue
    dependency_allowed "${dep}" "${allowed_addon_dependencies}" ||
      fail "artifact declares non-allowed dynamic dependency ${dep}: ${label}"
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
  local allowed_addon_dependencies="${3:-}"
  local file rel

  while IFS= read -r -d '' file; do
    rel="${file#"${root}"/}"
    check_dependencies_for_file \
      "${file}" "${label_prefix}/${rel}" "${allowed_addon_dependencies}"
  done < <(find "${root}" -type f -print0)
}

addon_dependencies_for_archive() {
  local relative_path="$1"
  local release_manifest
  release_manifest="${repo_root}/$(dirname "${relative_path}")/manifest.json"
  [[ -f "${release_manifest}" ]] ||
    fail "runtime addon archive has no adjacent manifest: ${relative_path}"

  python3 - "${release_manifest}" <<'PY'
import json
import pathlib
import sys

manifest = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
names = {
    pathlib.PurePosixPath(path).name
    for platform in manifest.get("platforms", [])
    for path in platform.get("bundledLibraries", [])
}
if manifest.get("userInstalledNativeDependencies") is True:
    # The v1 addon manifest's only approved ambient implementation dependency
    # is the documented POSIX libcurl contract. Windows packages absorb curl.
    names.update({"libcurl.so.4", "libcurl.4.dylib"})
for name in sorted(names):
    print(name)
PY
}

check_archive() {
  local relative_path="$1"
  local archive="${repo_root}/${relative_path}"
  local dest allowed_addon_dependencies=""

  [[ -f "${archive}" ]] || fail "missing artifact: ${relative_path}"
  case "${relative_path}" in
    runtime-addons/*/native/releases/*/*.tar.gz)
      allowed_addon_dependencies="$(addon_dependencies_for_archive "${relative_path}")"
      ;;
  esac
  dest="$(mktemp -d "${tmp_root}/archive.XXXXXX")"
  COPYFILE_DISABLE=1 tar -xzf "${archive}" -C "${dest}"
  check_tree "${dest}" "${relative_path}" "${allowed_addon_dependencies}"
}

require_command objdump
require_command tar
require_command awk
require_command python3

[[ -f "${manifest}" ]] || fail "missing manifest: artifacts/public-artifacts.tsv"
if [[ -n "${tmp_root}" ]]; then
  mkdir -p "${tmp_root}"
else
  mkdir -p "${repo_root}/.tmp"
  tmp_root="$(mktemp -d "${repo_root}/.tmp/native-linkage.XXXXXX")"
fi
trap 'rm -rf "${tmp_root}"' EXIT

while IFS=$'\t' read -r row_status label relative_path _expected_sha _extra || [[ -n "${row_status:-}" ]]; do
  [[ -z "${row_status:-}" || "${row_status}" == \#* ]] && continue
  [[ "${row_status}" == "public" ]] || continue
  case "${relative_path}" in
    logger/native/releases/*.tar.gz|runtime/native/releases/*.tar.gz|runtime-addons/*/native/releases/*/*.tar.gz|runtime-inspect/native/releases/*.tar.gz|cli/releases/*.tar.gz|demo/coakka-client/releases/*.tar.gz|coakka-tools/coakka-client/releases/*/*.tar.gz|coakka-tools/coakka-client/docker-demo/releases/*/*.tar.gz|coakka-tools/coakka-runtime-inspect/releases/*/*.tar.gz)
      check_archive "${relative_path}"
      ;;
  esac
done <"${manifest}"

if [[ -d "${repo_root}/native" ]]; then
  check_tree "${repo_root}/native" "native"
fi

echo "[native-artifact-linkage] ok"
