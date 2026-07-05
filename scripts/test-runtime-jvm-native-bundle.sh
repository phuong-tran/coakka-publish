#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tmp_parent="${COAKKA_PUBLIC_TMP_ROOT:-${repo_root}/.tmp}"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/coakka-runtime-jvm-bundle.XXXXXX")"
test_output="${tmp_root}/last-command.out"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

expect_success() {
  local label="$1"
  shift
  if ! "$@" >"${test_output}" 2>&1; then
    echo "[runtime-jvm-bundle-test] expected success: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

expect_failure() {
  local label="$1"
  shift
  if "$@" >"${test_output}" 2>&1; then
    echo "[runtime-jvm-bundle-test] expected failure: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

make_runtime_jar() {
  local target="$1"
  local fixture="$2"
  local native_version="$3"
  local macos_marker="$4"
  local root="${tmp_root}/jar-root"
  rm -rf "${root}"
  mkdir -p \
    "${root}/META-INF" \
    "${root}/native/linux-aarch64" \
    "${root}/native/linux-x86_64" \
    "${root}/native/macos-aarch64" \
    "${root}/native/windows-aarch64"

  cat >"${root}/META-INF/MANIFEST.MF" <<EOF
Manifest-Version: 1.0
Coakka-V2-Native-Package-Version: ${native_version}

EOF

  cp "${fixture}/native/linux-aarch64/libcoakka_runtime_v2.so" \
    "${root}/native/linux-aarch64/libcoakka_runtime_v2.so"
  cp "${fixture}/native/linux-aarch64/libcoakka_runtime_v2.so" \
    "${root}/native/linux-aarch64/libcoakka_runtime_v2-${native_version}.so"
  cp "${fixture}/native/linux-x86_64/libcoakka_runtime_v2.so" \
    "${root}/native/linux-x86_64/libcoakka_runtime_v2.so"
  cp "${fixture}/native/linux-x86_64/libcoakka_runtime_v2.so" \
    "${root}/native/linux-x86_64/libcoakka_runtime_v2-${native_version}.so"

  if [[ "${macos_marker}" == "stale" ]]; then
    printf 'macos-aarch64 stale\n' >"${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  else
    cp "${fixture}/native/macos-aarch64/libcoakka_runtime_v2.dylib" \
      "${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  fi
  cp "${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib" \
    "${root}/native/macos-aarch64/libcoakka_runtime_v2-${native_version}.dylib"

  cp "${fixture}/native/windows-aarch64/libcoakka_runtime_v2.dll" \
    "${root}/native/windows-aarch64/libcoakka_runtime_v2.dll"
  cp "${fixture}/native/windows-aarch64/libcoakka_runtime_v2.dll" \
    "${root}/native/windows-aarch64/libcoakka_runtime_v2-${native_version}.dll"

  (cd "${root}" && zip -qr "${target}" .)
}

make_native_archive() {
  local fixture="$1"
  local native_version="$2"
  local core_version="${native_version%%+*}"
  local archive_root="${tmp_root}/native-archive-root"
  local package_root="coakka-runtime-native-v2-${core_version}"

  rm -rf "${archive_root}"
  mkdir -p \
    "${archive_root}/${package_root}/native" \
    "${fixture}/runtime/native/releases/${native_version}"

  cp -R "${fixture}/native/." "${archive_root}/${package_root}/native/"

  (
    cd "${archive_root}"
    tar -czf \
      "${fixture}/runtime/native/releases/${native_version}/coakka-runtime-native-v2-${core_version}.tar.gz" \
      "${package_root}"
  )
}

write_public_manifest() {
  local fixture="$1"
  local native_version="$2"
  local jvm_version="$3"
  local native_core_version="${native_version%%+*}"

  mkdir -p "${fixture}/artifacts"
  cat >"${fixture}/artifacts/public-artifacts.tsv" <<EOF
# Public artifact manifest v1.
# Columns: status	label	relative_path	sha256
public	runtime Native package	runtime/native/releases/${native_version}/coakka-runtime-native-v2-${native_core_version}.tar.gz	0000000000000000000000000000000000000000000000000000000000000000
public	runtime JVM jar	runtime/jvm/releases/${native_version}/coakka-jvm-native-runtime-v2-${jvm_version}.jar	0000000000000000000000000000000000000000000000000000000000000000
EOF
}

make_fixture() {
  local name="$1"
  local release_marker="${2:-current}"
  local fixture="${tmp_root}/${name}"
  local native_version="0.1.0+test"
  local jvm_version="0.1.0-test"
  rm -rf "${fixture}"

  mkdir -p \
    "${fixture}/native/linux-aarch64" \
    "${fixture}/native/linux-x86_64" \
    "${fixture}/native/macos-aarch64" \
    "${fixture}/native/windows-aarch64" \
    "${fixture}/runtime/native/releases/${native_version}" \
    "${fixture}/runtime/jvm/releases/${native_version}" \
    "${fixture}/maven/coakka/v2/coakka-jvm-native-runtime-v2/0.1.0-test" \
    "${fixture}/scripts"

  cp "${repo_root}/scripts/verify-runtime-jvm-native-bundle.sh" \
    "${fixture}/scripts/verify-runtime-jvm-native-bundle.sh"
  chmod +x "${fixture}/scripts/verify-runtime-jvm-native-bundle.sh"

  printf 'linux-aarch64 current\n' >"${fixture}/native/linux-aarch64/libcoakka_runtime_v2.so"
  printf 'linux-x86_64 current\n' >"${fixture}/native/linux-x86_64/libcoakka_runtime_v2.so"
  printf 'macos-aarch64 current\n' >"${fixture}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  printf 'windows-aarch64 current\n' >"${fixture}/native/windows-aarch64/libcoakka_runtime_v2.dll"

  make_native_archive "${fixture}" "${native_version}"

  make_runtime_jar \
    "${fixture}/runtime/jvm/releases/${native_version}/coakka-jvm-native-runtime-v2-${jvm_version}.jar" \
    "${fixture}" \
    "${native_version}" \
    "${release_marker}"
  make_runtime_jar \
    "${fixture}/maven/coakka/v2/coakka-jvm-native-runtime-v2/${jvm_version}/coakka-jvm-native-runtime-v2-${jvm_version}.jar" \
    "${fixture}" \
    "${native_version}" \
    "current"
  write_public_manifest "${fixture}" "${native_version}" "${jvm_version}"

  printf '%s\n' "${fixture}"
}

good_fixture="$(make_fixture good)"
printf 'linux-aarch64 newer root\n' >"${good_fixture}/native/linux-aarch64/libcoakka_runtime_v2.so"
printf 'linux-x86_64 newer root\n' >"${good_fixture}/native/linux-x86_64/libcoakka_runtime_v2.so"
printf 'macos-aarch64 newer root\n' >"${good_fixture}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
printf 'windows-aarch64 newer root\n' >"${good_fixture}/native/windows-aarch64/libcoakka_runtime_v2.dll"
expect_success "matching runtime JVM bundle" "${good_fixture}/scripts/verify-runtime-jvm-native-bundle.sh"

mismatch_fixture="$(make_fixture mismatch stale)"
expect_failure "mismatched runtime JVM bundle" "${mismatch_fixture}/scripts/verify-runtime-jvm-native-bundle.sh"
grep -Fq "native entry mismatch" "${test_output}"

missing_entry_fixture="$(make_fixture missing-entry)"
zip -dq \
  "${missing_entry_fixture}/runtime/jvm/releases/0.1.0+test/coakka-jvm-native-runtime-v2-0.1.0-test.jar" \
  "native/linux-aarch64/libcoakka_runtime_v2-0.1.0+test.so"
expect_failure "missing versioned native entry" "${missing_entry_fixture}/scripts/verify-runtime-jvm-native-bundle.sh"
grep -Fq "missing native entry" "${test_output}"

stale_public_native_fixture="$(make_fixture stale-public-native)"
sed -i.bak \
  's:runtime/native/releases/0.1.0+test/:runtime/native/releases/0.1.0+newer/:' \
  "${stale_public_native_fixture}/artifacts/public-artifacts.tsv"
expect_failure "JVM jar behind public native package" "${stale_public_native_fixture}/scripts/verify-runtime-jvm-native-bundle.sh"
grep -Fq "public runtime native package is 0.1.0+newer" "${test_output}"

echo "[runtime-jvm-bundle-test] ok"
