#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-runtime-intake.XXXXXX")"
test_output="${tmp_root}/last-command.out"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

expect_success() {
  local label="$1"
  shift
  if ! "$@" >"${test_output}" 2>&1; then
    echo "[runtime-intake-test] expected success: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

expect_failure() {
  local label="$1"
  shift
  if "$@" >"${test_output}" 2>&1; then
    echo "[runtime-intake-test] expected failure: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

make_python_wheel() {
  local target="$1"
  local native_version="$2"
  local marker="$3"
  local root="${tmp_root}/wheel-root"
  rm -rf "${root}"
  mkdir -p "${root}/coakka_v2_connector/native/macos-aarch64"
  cat >"${root}/coakka_v2_connector/_packaging.py" <<EOF
COAKKA_V2_NATIVE_PACKAGE_VERSION = "${native_version}"
EOF
  printf '%s\n' "${marker}" >"${root}/coakka_v2_connector/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  (cd "${root}" && zip -qr "${target}" .)
}

good_wheel="${tmp_root}/coakka_v2_connector-0.1.0-py3-none-any.whl"
make_python_wheel "${good_wheel}" "0.1.0+63c346e" "public runtime binary placeholder"
expect_success \
  "clean Python wheel" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${good_wheel}" \
  --expected-native-version "0.1.0+63c346e"

bad_version_wheel="${tmp_root}/coakka_v2_connector-0.1.0-bad-version.whl"
make_python_wheel "${bad_version_wheel}" "0.1.0+old" "public runtime binary placeholder"
expect_failure \
  "wrong native version" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${bad_version_wheel}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "expected '0.1.0+63c346e'" "${test_output}"

private_marker_wheel="${tmp_root}/coakka_v2_connector-0.1.0-private.whl"
private_marker="coakka""Core"
make_python_wheel "${private_marker_wheel}" "0.1.0+63c346e" "private ${private_marker} marker"
expect_failure \
  "private marker" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${private_marker_wheel}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "${private_marker}" "${test_output}"

echo "[runtime-intake-test] ok"
