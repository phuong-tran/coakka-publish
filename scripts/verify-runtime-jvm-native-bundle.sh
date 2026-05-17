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
  local entry

  entry="$(
    tar -tzf "${archive_path}" |
      awk -v suffix="${entry_suffix}" '
        index($0, suffix) > 0 && index($0, suffix) == length($0) - length(suffix) + 1 {
          print
          exit
        }
      '
  )"
  [[ -n "${entry}" ]] || return 1
  tar -xOzf "${archive_path}" "${entry}" | shasum -a 256 | awk '{print $1}'
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

native_release_archive() {
  local native_version="$1"
  local release_dir="${repo_root}/runtime/native/releases/${native_version}"
  local archive

  [[ -d "${release_dir}" ]] || return 1
  archive="$(find "${release_dir}" -maxdepth 1 -type f -name 'coakka-runtime-native-v2-*.tar.gz' -print -quit)"
  [[ -n "${archive}" ]] || return 1
  printf '%s\n' "${archive}"
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
    fail "${jar_path#${repo_root}/} is missing native entry ${entry}"
  fi
  if [[ "${actual}" != "${expected}" ]]; then
    fail "${jar_path#${repo_root}/} native entry mismatch: ${entry}"
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

check_runtime_jvm_jar() {
  local jar_path="$1"
  local native_version
  local native_archive=""

  native_version="$(jar_native_version "${jar_path}")"
  [[ -n "${native_version}" ]] || fail "${jar_path#${repo_root}/} is missing Coakka-V2-Native-Package-Version"

  if [[ -x "${intake_verifier}" ]]; then
    "${intake_verifier}" \
      --lane jvm \
      --artifact "${jar_path}" \
      --expected-native-version "${native_version}" >/dev/null
  fi

  native_archive="$(native_release_archive "${native_version}" || true)"

  check_platform_entries \
    "${jar_path}" "${native_version}" "linux-aarch64" "libcoakka_runtime_v2" "so" "${native_archive}"
  check_platform_entries \
    "${jar_path}" "${native_version}" "linux-x86_64" "libcoakka_runtime_v2" "so" "${native_archive}"
  check_platform_entries \
    "${jar_path}" "${native_version}" "macos-aarch64" "libcoakka_runtime_v2" "dylib" "${native_archive}"
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
while IFS= read -r -d '' jar_path; do
  check_runtime_jvm_jar "${jar_path}"
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
