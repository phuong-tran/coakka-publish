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

native_core_version() {
  local native_version="$1"
  printf '%s\n' "${native_version%%+*}"
}

native_release_archive() {
  local native_version="$1"
  local core_version

  core_version="$(native_core_version "${native_version}")"
  printf '%s/runtime/native/releases/%s/coakka-runtime-native-v2-%s.tar.gz\n' \
    "${repo_root}" \
    "${native_version}" \
    "${core_version}"
}

sha256_native_release_entry() {
  local archive_path="$1"
  local native_version="$2"
  local platform="$3"
  local filename="$4"
  local core_version

  core_version="$(native_core_version "${native_version}")"
  tar -xOzf \
    "${archive_path}" \
    "coakka-runtime-native-v2-${core_version}/native/${platform}/${filename}" |
    shasum -a 256 |
    awk '{print $1}'
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

check_entry_matches_expected() {
  local jar_path="$1"
  local entry="$2"
  local expected="$3"
  local actual

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
  local filename="${basename}.${extension}"
  local native_archive
  local root_native="${repo_root}/native/${platform}/${basename}.${extension}"
  local expected

  native_archive="$(native_release_archive "${native_version}")"
  if [[ -f "${native_archive}" ]]; then
    if ! expected="$(sha256_native_release_entry "${native_archive}" "${native_version}" "${platform}" "${filename}")"; then
      fail "${native_archive#${repo_root}/} is missing native entry native/${platform}/${filename}"
    fi
  else
    [[ -f "${root_native}" ]] || fail "missing root native library: native/${platform}/${filename}"
    expected="$(sha256_file "${root_native}")"
  fi

  check_entry_matches_expected "${jar_path}" "native/${platform}/${filename}" "${expected}"
  check_entry_matches_expected "${jar_path}" "native/${platform}/${basename}-${native_version}.${extension}" "${expected}"
}

check_runtime_jvm_jar() {
  local jar_path="$1"
  local native_version

  native_version="$(jar_native_version "${jar_path}")"
  [[ -n "${native_version}" ]] || fail "${jar_path#${repo_root}/} is missing Coakka-V2-Native-Package-Version"

  if [[ -x "${intake_verifier}" ]]; then
    "${intake_verifier}" \
      --lane jvm \
      --artifact "${jar_path}" \
      --expected-native-version "${native_version}" >/dev/null
  fi

  check_platform_entries "${jar_path}" "${native_version}" "linux-aarch64" "libcoakka_runtime_v2" "so"
  check_platform_entries "${jar_path}" "${native_version}" "linux-x86_64" "libcoakka_runtime_v2" "so"
  check_platform_entries "${jar_path}" "${native_version}" "macos-aarch64" "libcoakka_runtime_v2" "dylib"
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
