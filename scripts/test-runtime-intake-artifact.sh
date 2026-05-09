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

make_jvm_jar() {
  local target="$1"
  local native_version="$2"
  local marker="$3"
  local root="${tmp_root}/jvm-root"
  rm -rf "${root}"
  mkdir -p "${root}/META-INF" "${root}/native/macos-aarch64"
  cat >"${root}/META-INF/MANIFEST.MF" <<EOF
Manifest-Version: 1.0
Coakka-V2-Native-Package-Version: ${native_version}

EOF
  printf '%s\n' "${marker}" >"${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  (cd "${root}" && zip -qr "${target}" .)
}

make_node_package() {
  local target="$1"
  local native_version="$2"
  local marker="$3"
  local root="${tmp_root}/node-root/package"
  rm -rf "${tmp_root}/node-root"
  mkdir -p "${root}/dist" "${root}/native/macos-aarch64"
  cat >"${root}/dist/packaging.js" <<EOF
export const COAKKA_V2_NATIVE_PACKAGE_VERSION = "${native_version}";
EOF
  printf '%s\n' "${marker}" >"${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  tar -C "${tmp_root}/node-root" -czf "${target}" package
}

make_go_package() {
  local target="$1"
  local native_version="$2"
  local marker="$3"
  local root="${tmp_root}/go-root/coakka-v2-connector-go-0.1.0"
  rm -rf "${tmp_root}/go-root"
  mkdir -p "${root}/native/macos-aarch64"
  cat >"${root}/packaging.go" <<EOF
package coakka

const CoakkaV2NativePackageVersion = "${native_version}"
EOF
  printf '%s\n' "${marker}" >"${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  tar -C "${tmp_root}/go-root" -czf "${target}" coakka-v2-connector-go-0.1.0
}

make_csharp_package() {
  local target="$1"
  local native_version="$2"
  local marker="$3"
  local root="${tmp_root}/csharp-root"
  rm -rf "${root}"
  mkdir -p "${root}/coakka/metadata" "${root}/contentFiles/any/any/native/macos-aarch64"
  cat >"${root}/coakka/metadata/runtime-package.json" <<EOF
{"bundled_native_package_version":"${native_version}"}
EOF
  printf '%s\n' "${marker}" >"${root}/contentFiles/any/any/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  (cd "${root}" && zip -qr "${target}" .)
}

make_rust_package() {
  local target="$1"
  local native_version="$2"
  local marker="$3"
  local root="${tmp_root}/rust-root/coakka-runtime-rs-0.1.0"
  rm -rf "${tmp_root}/rust-root"
  mkdir -p "${root}/native/macos-aarch64"
  cat >"${root}/coakka-runtime-package.json" <<EOF
{"bundled_native_package_version":"${native_version}"}
EOF
  printf '%s\n' "${marker}" >"${root}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  tar -C "${tmp_root}/rust-root" -czf "${target}" coakka-runtime-rs-0.1.0
}

verify_intake() {
  local label="$1"
  local lane="$2"
  local artifact="$3"
  expect_success \
    "${label}" \
    "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
    --lane "${lane}" \
    --artifact "${artifact}" \
    --expected-native-version "0.1.0+63c346e"
}

good_jvm="${tmp_root}/coakka-jvm-native-runtime-v2-0.1.0.jar"
make_jvm_jar "${good_jvm}" "0.1.0+63c346e" "public runtime binary placeholder"
verify_intake "clean JVM jar" jvm "${good_jvm}"

jvm_sources="${tmp_root}/coakka-jvm-native-runtime-v2-0.1.0-sources.jar"
jvm_sources_root="${tmp_root}/jvm-sources-root"
rm -rf "${jvm_sources_root}"
mkdir -p "${jvm_sources_root}/META-INF" "${jvm_sources_root}/coakka/v2/connector"
cat >"${jvm_sources_root}/META-INF/MANIFEST.MF" <<'EOF'
Manifest-Version: 1.0
Coakka-V2-Native-Package-Version: 0.1.0+63c346e

EOF
printf 'package coakka.v2.connector;\n' >"${jvm_sources_root}/coakka/v2/connector/RuntimeHost.java"
(cd "${jvm_sources_root}" && zip -qr "${jvm_sources}" .)
expect_success \
  "clean JVM sources jar" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane jvm \
  --artifact "${jvm_sources}" \
  --expected-native-version "0.1.0+63c346e" \
  --allow-no-native

expect_failure \
  "main JVM jar without native" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane jvm \
  --artifact "${jvm_sources}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "does not include any runtime native library entries" "${test_output}"

good_wheel="${tmp_root}/coakka_v2_connector-0.1.0-py3-none-any.whl"
make_python_wheel "${good_wheel}" "0.1.0+63c346e" "public runtime binary placeholder"
verify_intake "clean Python wheel" python "${good_wheel}"

good_node="${tmp_root}/coakka-v2-connector-node-0.1.0.tgz"
make_node_package "${good_node}" "0.1.0+63c346e" "public runtime binary placeholder"
verify_intake "clean Node package" node "${good_node}"

good_go="${tmp_root}/coakka-v2-connector-go-0.1.0.tar.gz"
make_go_package "${good_go}" "0.1.0+63c346e" "public runtime binary placeholder"
verify_intake "clean Go package" go "${good_go}"

good_csharp="${tmp_root}/CoAkka.Runtime.0.1.0.nupkg"
make_csharp_package "${good_csharp}" "0.1.0+63c346e" "public runtime binary placeholder"
verify_intake "clean C# package" csharp "${good_csharp}"

good_rust="${tmp_root}/coakka-runtime-rs-0.1.0.tar.gz"
make_rust_package "${good_rust}" "0.1.0+63c346e" "public runtime binary placeholder"
verify_intake "clean Rust package" rust "${good_rust}"

bad_version_wheel="${tmp_root}/coakka_v2_connector-0.1.0-bad-version.whl"
make_python_wheel "${bad_version_wheel}" "0.1.0+old" "public runtime binary placeholder"
expect_failure \
  "wrong native version" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${bad_version_wheel}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "expected '0.1.0+63c346e'" "${test_output}"

blocked_marker_wheel="${tmp_root}/coakka_v2_connector-0.1.0-blocked-marker.whl"
blocked_marker="$(printf '%s/%s/%s/pkg' "/Users" "builder" "workspace")"
make_python_wheel "${blocked_marker_wheel}" "0.1.0+63c346e" "blocked ${blocked_marker} marker"
expect_failure \
  "blocked marker" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${blocked_marker_wheel}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "forbidden marker" "${test_output}"

bad_component_wheel="${tmp_root}/coakka_v2_connector-0.1.0-test-path.whl"
bad_component_root="${tmp_root}/bad-component-root"
rm -rf "${bad_component_root}"
mkdir -p "${bad_component_root}/coakka_v2_connector/native/macos-aarch64" "${bad_component_root}/tests"
cat >"${bad_component_root}/coakka_v2_connector/_packaging.py" <<'EOF'
COAKKA_V2_NATIVE_PACKAGE_VERSION = "0.1.0+63c346e"
EOF
printf 'public runtime binary placeholder\n' >"${bad_component_root}/coakka_v2_connector/native/macos-aarch64/libcoakka_runtime_v2.dylib"
printf 'should not ship\n' >"${bad_component_root}/tests/leak.txt"
(cd "${bad_component_root}" && zip -qr "${bad_component_wheel}" .)
expect_failure \
  "forbidden test path" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${bad_component_wheel}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "workspace/demo/test path leaked" "${test_output}"

bad_native_name_wheel="${tmp_root}/coakka_v2_connector-0.1.0-stale-native.whl"
bad_native_root="${tmp_root}/bad-native-root"
rm -rf "${bad_native_root}"
mkdir -p "${bad_native_root}/coakka_v2_connector/native/macos-aarch64"
cat >"${bad_native_root}/coakka_v2_connector/_packaging.py" <<'EOF'
COAKKA_V2_NATIVE_PACKAGE_VERSION = "0.1.0+63c346e"
EOF
printf 'public runtime binary placeholder\n' >"${bad_native_root}/coakka_v2_connector/native/macos-aarch64/libcoakka_runtime_v2-0.1.0+old.dylib"
(cd "${bad_native_root}" && zip -qr "${bad_native_name_wheel}" .)
expect_failure \
  "stale native name" \
  "${repo_root}/scripts/verify-runtime-intake-artifact.py" \
  --lane python \
  --artifact "${bad_native_name_wheel}" \
  --expected-native-version "0.1.0+63c346e"
grep -Fq "stale or forbidden native library entry" "${test_output}"

echo "[runtime-intake-test] ok"
