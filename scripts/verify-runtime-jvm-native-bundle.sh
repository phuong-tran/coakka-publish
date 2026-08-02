#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
intake_verifier="${repo_root}/scripts/verify-runtime-intake-artifact.py"

fail() {
  echo "[verify-runtime-jvm-native-bundle] $*" >&2
  exit 1
}

sha256_file() {
  shasum -a 256 "$1" | awk '{print $1}'
}

sha256_jar_entry() {
  local jar_path="$1"
  local entry="$2"
  unzip -p "${jar_path}" "${entry}" | shasum -a 256 | awk '{print $1}'
}

sha256_tar_entry() {
  local archive_path="$1"
  local entry_suffix="$2"

  python3 - "${archive_path}" "${entry_suffix}" <<'PY'
import hashlib
import sys
import tarfile

archive_path, entry_suffix = sys.argv[1], sys.argv[2]
with tarfile.open(archive_path, "r:gz") as archive:
    for member in archive.getmembers():
        if not member.isfile() or not member.name.endswith(entry_suffix):
            continue
        extracted = archive.extractfile(member)
        if extracted is None:
            raise SystemExit(1)
        print(hashlib.sha256(extracted.read()).hexdigest())
        raise SystemExit(0)
raise SystemExit(1)
PY
}

jar_has_entry() {
  local jar_path="$1"
  local entry="$2"
  unzip -Z1 "${jar_path}" "${entry}" >/dev/null 2>&1
}

archive_has_entry_suffix() {
  local archive_path="$1"
  local entry_suffix="$2"

  [[ -n "${archive_path}" ]] || return 1
  python3 - "${archive_path}" "${entry_suffix}" <<'PY'
import sys
import tarfile

archive_path, entry_suffix = sys.argv[1], sys.argv[2]
with tarfile.open(archive_path, "r:gz") as archive:
    for member in archive.getmembers():
        if member.name.endswith(entry_suffix):
            raise SystemExit(0)
raise SystemExit(1)
PY
}

jar_native_version() {
  local jar_path="$1"
  unzip -p "${jar_path}" META-INF/MANIFEST.MF |
    awk -F': ' '
      /^Coakka-V2-Native-Package-Version:/ {
        gsub(/\r$/, "", $2)
        print $2
        exit
      }
    '
}

public_runtime_native_version() {
  local manifest="${repo_root}/artifacts/public-artifacts.tsv"
  local version=""
  local count=0

  [[ -f "${manifest}" ]] || return 1
  while IFS=$'\t' read -r _status _label relative_path _sha; do
    [[ -n "${relative_path}" ]] || continue
    case "${relative_path}" in
      runtime/native/releases/*/coakka-runtime-native-v2-*.tar.gz)
        version="${relative_path#runtime/native/releases/}"
        version="${version%%/*}"
        count=$((count + 1))
        ;;
    esac
  done < <(awk -F '\t' '$2 == "runtime Native package" { print }' "${manifest}")

  [[ "${count}" -le 1 ]] || fail "artifacts/public-artifacts.tsv has multiple runtime Native package rows"
  [[ "${count}" -eq 1 ]] || return 1
  printf '%s\n' "${version}"
}

native_release_archive() {
  local native_version="$1"
  local release_dir="${repo_root}/runtime/native/releases/${native_version}"
  local archive

  [[ -d "${release_dir}" ]] || return 1
  archive="$(find "${release_dir}" -maxdepth 1 -type f -name 'coakka-runtime-native-v2-*.tar.gz' -print -quit)"
  [[ -n "${archive}" ]] || return 1
  printf '%s\n' "${archive}"
}

native_release_manifest() {
  local native_version="$1"
  local manifest="${repo_root}/runtime/native/releases/${native_version}/manifest.json"

  [[ -f "${manifest}" ]] || return 1
  printf '%s\n' "${manifest}"
}

manifest_platforms() {
  local manifest="$1"

  python3 - "${manifest}" <<'PY'
import json
import sys

manifest_path = sys.argv[1]
supported = [
    "linux-aarch64",
    "linux-x86_64",
    "macos-aarch64",
    "windows-aarch64",
    "windows-x86_64",
]

with open(manifest_path, "r", encoding="utf-8") as handle:
    manifest = json.load(handle)

platforms = manifest.get("platforms")
if not isinstance(platforms, list) or not platforms:
    raise SystemExit("native release manifest requires a non-empty platform matrix")
if platforms != [platform for platform in supported if platform in platforms]:
    raise SystemExit("native release manifest platform matrix is unsupported or non-canonical")
for platform in platforms:
    print(platform)
PY
}

check_entry_matches_native() {
  local jar_path="$1"
  local entry="$2"
  local native_version="$3"
  local root_native="$4"
  local native_archive="$5"
  local native_entry="${6:-${entry}}"
  local expected actual

  if [[ -n "${native_archive}" ]]; then
    expected="$(sha256_tar_entry "${native_archive}" "${native_entry}")" ||
      fail "native release ${native_version} is missing ${native_entry}"
  else
    expected="$(sha256_file "${root_native}")"
  fi
  if ! actual="$(sha256_jar_entry "${jar_path}" "${entry}")"; then
    fail "${jar_path#"${repo_root}"/} is missing native entry ${entry}"
  fi
  if [[ "${actual}" != "${expected}" ]]; then
    fail "${jar_path#"${repo_root}"/} native entry mismatch: ${entry}"
  fi
}

check_platform_entries() {
  local jar_path="$1"
  local native_version="$2"
  local platform="$3"
  local basename="$4"
  local extension="$5"
  local root_native="${repo_root}/native/${platform}/${basename}.${extension}"
  local native_archive="${6:-}"

  if [[ -z "${native_archive}" ]]; then
    [[ -f "${root_native}" ]] || fail "missing root native library: native/${platform}/${basename}.${extension}"
  fi
  check_entry_matches_native \
    "${jar_path}" \
    "native/${platform}/${basename}.${extension}" \
    "${native_version}" \
    "${root_native}" \
    "${native_archive}"
  check_entry_matches_native \
    "${jar_path}" \
    "native/${platform}/${basename}-${native_version}.${extension}" \
    "${native_version}" \
    "${root_native}" \
    "${native_archive}" \
    "native/${platform}/${basename}.${extension}"
}

check_manifest_platform_entries() {
  local jar_path="$1"
  local native_version="$2"
  local native_archive="$3"
  local manifest="$4"
  local selected=$'\n'
  local platform basename extension

  while IFS= read -r platform; do
    [[ -n "${platform}" ]] || continue
    selected+="${platform}"$'\n'
    case "${platform}" in
      linux-*) basename="libcoakka_runtime_v2"; extension="so" ;;
      macos-*) basename="libcoakka_runtime_v2"; extension="dylib" ;;
      windows-*) basename="libcoakka_runtime_v2"; extension="dll" ;;
      *) fail "unsupported native release platform: ${platform}" ;;
    esac
    check_platform_entries \
      "${jar_path}" "${native_version}" "${platform}" \
      "${basename}" "${extension}" "${native_archive}"
  done < <(manifest_platforms "${manifest}")

  for platform in \
    linux-aarch64 linux-x86_64 macos-aarch64 windows-aarch64 windows-x86_64; do
    [[ "${selected}" == *$'\n'"${platform}"$'\n'* ]] && continue
    case "${platform}" in
      linux-*) extension="so" ;;
      macos-*) extension="dylib" ;;
      windows-*) extension="dll" ;;
    esac
    if jar_has_entry "${jar_path}" "native/${platform}/libcoakka_runtime_v2.${extension}" ||
       jar_has_entry "${jar_path}" "native/${platform}/libcoakka_runtime_v2-${native_version}.${extension}"; then
      fail "${jar_path#"${repo_root}"/} contains native platform outside release manifest: ${platform}"
    fi
  done
}

check_runtime_jvm_jar() {
  local jar_path="$1"
  local expected_public_native_version="${2:-}"
  local native_version
  local native_archive=""
  local native_manifest=""

  native_version="$(jar_native_version "${jar_path}")"
  [[ -n "${native_version}" ]] || fail "${jar_path#"${repo_root}"/} is missing Coakka-V2-Native-Package-Version"
  if [[ -n "${expected_public_native_version}" && "${native_version}" != "${expected_public_native_version}" ]]; then
    fail "${jar_path#"${repo_root}"/} bundles native package ${native_version}; public runtime native package is ${expected_public_native_version}"
  fi

  if [[ -x "${intake_verifier}" ]]; then
    "${intake_verifier}" \
      --lane jvm \
      --artifact "${jar_path}" \
      --expected-native-version "${native_version}" >/dev/null
  fi

  native_archive="$(native_release_archive "${native_version}" || true)"

  native_manifest="$(native_release_manifest "${native_version}" || true)"
  if [[ -n "${native_manifest}" ]]; then
    check_manifest_platform_entries \
      "${jar_path}" "${native_version}" "${native_archive}" "${native_manifest}"
    return 0
  fi

  check_platform_entries \
    "${jar_path}" "${native_version}" "linux-aarch64" "libcoakka_runtime_v2" "so" "${native_archive}"
  check_platform_entries \
    "${jar_path}" "${native_version}" "linux-x86_64" "libcoakka_runtime_v2" "so" "${native_archive}"
  check_platform_entries \
    "${jar_path}" "${native_version}" "macos-aarch64" "libcoakka_runtime_v2" "dylib" "${native_archive}"

  if jar_has_entry "${jar_path}" "native/windows-aarch64/libcoakka_runtime_v2.dll" || \
    jar_has_entry "${jar_path}" "native/windows-aarch64/libcoakka_runtime_v2-${native_version}.dll"; then
    local windows_native_archive=""
    if archive_has_entry_suffix "${native_archive}" "native/windows-aarch64/libcoakka_runtime_v2.dll"; then
      windows_native_archive="${native_archive}"
    fi
    check_platform_entries \
      "${jar_path}" "${native_version}" "windows-aarch64" "libcoakka_runtime_v2" "dll" "${windows_native_archive}"
  fi
  if jar_has_entry "${jar_path}" "native/windows-x86_64/libcoakka_runtime_v2.dll" || \
    jar_has_entry "${jar_path}" "native/windows-x86_64/libcoakka_runtime_v2-${native_version}.dll"; then
    local windows_x8664_native_archive=""
    if archive_has_entry_suffix "${native_archive}" "native/windows-x86_64/libcoakka_runtime_v2.dll"; then
      windows_x8664_native_archive="${native_archive}"
    fi
    check_platform_entries \
      "${jar_path}" "${native_version}" "windows-x86_64" "libcoakka_runtime_v2" "dll" "${windows_x8664_native_archive}"
  fi
}

current_runtime_jvm_jars() {
  local manifest="${repo_root}/artifacts/public-artifacts.tsv"
  local metadata="${repo_root}/maven/coakka/v2/coakka-jvm-native-runtime-v2/maven-metadata.xml"
  local latest

  if [[ -f "${manifest}" ]]; then
    awk -F '\t' '$2 == "runtime JVM jar" { print }' "${manifest}" |
      while IFS=$'\t' read -r _status _label relative_path _sha; do
        [[ -n "${relative_path}" ]] || continue
        printf '%s\0' "${repo_root}/${relative_path}"
      done
  fi

  if [[ -f "${metadata}" ]]; then
    latest="$(
      sed -n 's:.*<latest>\(.*\)</latest>.*:\1:p' "${metadata}" |
        sed -n '1p'
    )"
    if [[ -n "${latest}" ]]; then
      find \
        "${repo_root}/maven/coakka/v2/coakka-jvm-native-runtime-v2/${latest}" \
        -type f \
        -name 'coakka-jvm-native-runtime-v2-*.jar' \
        ! -name '*-sources.jar' \
        -print0 2>/dev/null || true
    fi
  fi
}

jar_count=0
public_native_version="$(public_runtime_native_version || true)"
while IFS= read -r -d '' jar_path; do
  check_runtime_jvm_jar "${jar_path}" "${public_native_version}"
  jar_count=$((jar_count + 1))
done < <(current_runtime_jvm_jars)

if [[ "${jar_count}" -eq 0 ]]; then
  while IFS= read -r -d '' jar_path; do
    check_runtime_jvm_jar "${jar_path}"
    jar_count=$((jar_count + 1))
  done < <(
    find \
      "${repo_root}/runtime/jvm/releases" \
      "${repo_root}/maven/coakka/v2/coakka-jvm-native-runtime-v2" \
      -type f \
      -name 'coakka-jvm-native-runtime-v2-*.jar' \
      ! -name '*-sources.jar' \
      -print0
  )
fi

if [[ "${jar_count}" -eq 0 ]]; then
  fail "no runtime JVM jars found"
fi

echo "[verify-runtime-jvm-native-bundle] ok"
