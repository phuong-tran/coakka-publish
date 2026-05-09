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

check_entry_matches_root() {
  local jar_path="$1"
  local entry="$2"
  local root_native="$3"
  local expected actual

  expected="$(sha256_file "${root_native}")"
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

  [[ -f "${root_native}" ]] || fail "missing root native library: native/${platform}/${basename}.${extension}"
  check_entry_matches_root "${jar_path}" "native/${platform}/${basename}.${extension}" "${root_native}"
  check_entry_matches_root "${jar_path}" "native/${platform}/${basename}-${native_version}.${extension}" "${root_native}"
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

jar_count=0
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

if [[ "${jar_count}" -eq 0 ]]; then
  fail "no runtime JVM jars found"
fi

echo "[verify-runtime-jvm-native-bundle] ok"
