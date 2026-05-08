#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-manifest.XXXXXX")"
test_output="${tmp_root}/last-command.out"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

expect_success() {
  local label="$1"
  shift
  if ! "$@" >"${test_output}" 2>&1; then
    echo "[public-manifest-test] expected success: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

expect_failure() {
  local label="$1"
  shift
  if "$@" >"${test_output}" 2>&1; then
    echo "[public-manifest-test] expected failure: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

sha256_file() {
  shasum -a 256 "$1" | awk '{print $1}'
}

make_fixture() {
  local name="$1"
  local fixture="${tmp_root}/${name}"
  rm -rf "${fixture}"

  mkdir -p \
    "${fixture}/artifacts" \
    "${fixture}/docs" \
    "${fixture}/include/coakka/v2" \
    "${fixture}/logger/native/releases/test" \
    "${fixture}/maven" \
    "${fixture}/native/linux-aarch64" \
    "${fixture}/native/linux-x86_64" \
    "${fixture}/native/macos-aarch64" \
    "${fixture}/runtime/native/releases/0.1.0+63c346e" \
    "${fixture}/scripts"

  cp "${repo_root}/scripts/verify-public-surface.sh" "${fixture}/scripts/verify-public-surface.sh"
  cat >"${fixture}/scripts/scan-public-surface.sh" <<'EOF'
#!/usr/bin/env bash
echo "[public-artifact-surface] ok"
EOF
  chmod +x "${fixture}/scripts/scan-public-surface.sh" "${fixture}/scripts/verify-public-surface.sh"

  printf '# Fixture\n' >"${fixture}/README.md"
  printf '# Fixture contract\n' >"${fixture}/docs/public-artifact-contract.md"
  printf 'client header\n' >"${fixture}/include/coakka/v2/client.h"
  printf 'control header\n' >"${fixture}/include/coakka/v2/control.h"
  printf 'runtime header\n' >"${fixture}/include/coakka/v2/runtime.h"
  printf 'transport header\n' >"${fixture}/include/coakka/v2/transport.h"
  printf 'utils header\n' >"${fixture}/include/coakka/v2/utils.h"
  printf 'linux aarch64 native\n' >"${fixture}/native/linux-aarch64/libcoakka_runtime_v2.so"
  printf 'linux x86_64 native\n' >"${fixture}/native/linux-x86_64/libcoakka_runtime_v2.so"
  printf 'macos aarch64 native\n' >"${fixture}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  printf 'logger archive\n' >"${fixture}/logger/native/releases/test/coakka-logger-native-test.tar.gz"
  printf 'runtime archive\n' >"${fixture}/runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz"

  (
    cd "${fixture}"
    shasum -a 256 \
      include/coakka/v2/client.h \
      include/coakka/v2/control.h \
      include/coakka/v2/runtime.h \
      include/coakka/v2/transport.h \
      include/coakka/v2/utils.h \
      native/linux-aarch64/libcoakka_runtime_v2.so \
      native/linux-x86_64/libcoakka_runtime_v2.so \
      native/macos-aarch64/libcoakka_runtime_v2.dylib >SHA256SUMS
  )
  (
    cd "${fixture}/logger/native/releases/test"
    shasum -a 256 coakka-logger-native-test.tar.gz >SHA256SUMS
  )
  (
    cd "${fixture}/runtime/native/releases/0.1.0+63c346e"
    shasum -a 256 coakka-runtime-native-v2-0.1.0.tar.gz >SHA256SUMS
  )

  local logger_sha runtime_sha
  logger_sha="$(sha256_file "${fixture}/logger/native/releases/test/coakka-logger-native-test.tar.gz")"
  runtime_sha="$(sha256_file "${fixture}/runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz")"
  cat >"${fixture}/artifacts/public-artifacts.tsv" <<EOF
# Public artifact manifest v1.
# Columns: status	label	relative_path	sha256
public	logger Native package	logger/native/releases/test/coakka-logger-native-test.tar.gz	${logger_sha}
public	runtime Native package	runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz	${runtime_sha}
EOF

  printf '%s\n' "${fixture}"
}

good_fixture="$(make_fixture good)"
expect_success "clean public manifest" "${good_fixture}/scripts/verify-public-surface.sh"

bad_path_fixture="$(make_fixture bad-path)"
cat >>"${bad_path_fixture}/artifacts/public-artifacts.tsv" <<'EOF'
public	unsafe path	../outside.tar.gz	0000000000000000000000000000000000000000000000000000000000000000
EOF
expect_failure "path outside public manifest surface" "${bad_path_fixture}/scripts/verify-public-surface.sh"
grep -Fq "unsafe artifact path" "${test_output}"

duplicate_path_fixture="$(make_fixture duplicate-path)"
runtime_sha="$(sha256_file "${duplicate_path_fixture}/runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz")"
cat >>"${duplicate_path_fixture}/artifacts/public-artifacts.tsv" <<EOF
public	runtime Native package duplicate	runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz	${runtime_sha}
EOF
expect_failure "duplicate manifest path" "${duplicate_path_fixture}/scripts/verify-public-surface.sh"
grep -Fq "duplicate artifact path" "${test_output}"

duplicate_label_fixture="$(make_fixture duplicate-label)"
logger_sha="$(sha256_file "${duplicate_label_fixture}/logger/native/releases/test/coakka-logger-native-test.tar.gz")"
cat >>"${duplicate_label_fixture}/artifacts/public-artifacts.tsv" <<EOF
public	logger Native package	logger/native/releases/test/coakka-logger-native-test-copy.tar.gz	${logger_sha}
EOF
expect_failure "duplicate manifest label" "${duplicate_label_fixture}/scripts/verify-public-surface.sh"
grep -Fq "duplicate artifact label" "${test_output}"

bad_sha_fixture="$(make_fixture bad-sha)"
sed -i.bak 's/[0-9a-f]\{64\}$/ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff/' \
  "${bad_sha_fixture}/artifacts/public-artifacts.tsv"
expect_failure "manifest checksum mismatch" "${bad_sha_fixture}/scripts/verify-public-surface.sh"
grep -Fq "manifest sha256 mismatch" "${test_output}"

echo "[public-manifest-test] ok"
