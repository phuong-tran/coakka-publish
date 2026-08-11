#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tmp_parent="${COAKKA_PUBLIC_TMP_ROOT:-${repo_root}/.tmp}"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/coakka-public-manifest.XXXXXX")"
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
    "${fixture}/coakka-tools/coakka-client/releases/1.3.1+2215b0f" \
    "${fixture}/coakka-tools/coakka-client/docker-demo/releases/1.3.1+2215b0f" \
    "${fixture}/logger/native/releases/test" \
    "${fixture}/maven" \
    "${fixture}/native/linux-aarch64" \
    "${fixture}/native/linux-x86_64" \
    "${fixture}/native/macos-aarch64" \
    "${fixture}/native/windows-aarch64" \
    "${fixture}/native/windows-x86_64" \
    "${fixture}/runtime/native/releases/0.1.0+63c346e" \
    "${fixture}/runtime-addons" \
    "${fixture}/coakka-tools/coakka-runtime-inspect/releases/1.3.1+e664986" \
    "${fixture}/scripts"

  cp "${repo_root}/scripts/verify-public-surface.sh" "${fixture}/scripts/verify-public-surface.sh"
  cp "${repo_root}/scripts/verify-runtime-addon-release.py" \
    "${fixture}/scripts/verify-runtime-addon-release.py"
  cat >"${fixture}/scripts/scan-public-surface.sh" <<'EOF'
#!/usr/bin/env bash
echo "[public-artifact-surface] ok"
EOF
  chmod +x \
    "${fixture}/scripts/scan-public-surface.sh" \
    "${fixture}/scripts/verify-public-surface.sh" \
    "${fixture}/scripts/verify-runtime-addon-release.py"

  printf '# Fixture\n' >"${fixture}/README.md"
  cp "${repo_root}/LICENSE.md" "${fixture}/LICENSE.md"
  printf '# Fixture contract\n' >"${fixture}/docs/public-artifact-contract.md"
  printf '# Runtime addons\n' >"${fixture}/runtime-addons/README.md"
  printf '{}\n' >"${fixture}/runtime-addons/manifest.schema.json"
  printf 'client header\n' >"${fixture}/include/coakka/v2/client.h"
  printf 'control header\n' >"${fixture}/include/coakka/v2/control.h"
  printf 'runtime header\n' >"${fixture}/include/coakka/v2/runtime.h"
  printf 'transport header\n' >"${fixture}/include/coakka/v2/transport.h"
  printf 'utils header\n' >"${fixture}/include/coakka/v2/utils.h"
  printf 'linux aarch64 native\n' >"${fixture}/native/linux-aarch64/libcoakka_runtime_v2.so"
  printf 'linux x86_64 native\n' >"${fixture}/native/linux-x86_64/libcoakka_runtime_v2.so"
  printf 'macos aarch64 native\n' >"${fixture}/native/macos-aarch64/libcoakka_runtime_v2.dylib"
  printf 'windows aarch64 native\n' >"${fixture}/native/windows-aarch64/libcoakka_runtime_v2.dll"
  printf 'windows x86_64 native\n' >"${fixture}/native/windows-x86_64/libcoakka_runtime_v2.dll"
  printf 'logger archive\n' >"${fixture}/logger/native/releases/test/coakka-logger-native-test.tar.gz"
  printf 'runtime archive\n' >"${fixture}/runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz"
  cat >"${fixture}/runtime/native/releases/0.1.0+63c346e/manifest.json" <<'EOF'
{
  "archive": "coakka-runtime-native-v2-0.1.0.tar.gz",
  "platforms": [
    "linux-aarch64",
    "linux-x86_64",
    "macos-aarch64",
    "windows-aarch64",
    "windows-x86_64"
  ]
}
EOF
  printf 'runtime inspect archive\n' >"${fixture}/coakka-tools/coakka-runtime-inspect/releases/1.3.1+e664986/coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz"
  printf 'cli archive\n' >"${fixture}/coakka-tools/coakka-client/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-linux-x86_64.tar.gz"
  printf 'docker demo archive\n' >"${fixture}/coakka-tools/coakka-client/docker-demo/releases/1.3.1+2215b0f/coakka-client-docker-demo-v2-1.3.1-linux-x86_64.tar.gz"

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
      native/macos-aarch64/libcoakka_runtime_v2.dylib \
      native/windows-aarch64/libcoakka_runtime_v2.dll \
      native/windows-x86_64/libcoakka_runtime_v2.dll >SHA256SUMS
  )
  (
    cd "${fixture}/logger/native/releases/test"
    shasum -a 256 coakka-logger-native-test.tar.gz >SHA256SUMS
  )
  (
    cd "${fixture}/runtime/native/releases/0.1.0+63c346e"
    shasum -a 256 \
      coakka-runtime-native-v2-0.1.0.tar.gz \
      manifest.json >SHA256SUMS
  )
  (
    cd "${fixture}/coakka-tools/coakka-runtime-inspect/releases/1.3.1+e664986"
    shasum -a 256 coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz >SHA256SUMS
  )
  (
    cd "${fixture}/coakka-tools/coakka-client/releases/1.3.1+2215b0f"
    shasum -a 256 coakka-client-v2-1.3.1-linux-x86_64.tar.gz >SHA256SUMS
  )
  (
    cd "${fixture}/coakka-tools/coakka-client/docker-demo/releases/1.3.1+2215b0f"
    shasum -a 256 coakka-client-docker-demo-v2-1.3.1-linux-x86_64.tar.gz >SHA256SUMS
  )

  local logger_sha runtime_sha inspect_sha cli_sha demo_sha
  logger_sha="$(sha256_file "${fixture}/logger/native/releases/test/coakka-logger-native-test.tar.gz")"
  runtime_sha="$(sha256_file "${fixture}/runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz")"
  inspect_sha="$(sha256_file "${fixture}/coakka-tools/coakka-runtime-inspect/releases/1.3.1+e664986/coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz")"
  cli_sha="$(sha256_file "${fixture}/coakka-tools/coakka-client/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-linux-x86_64.tar.gz")"
  demo_sha="$(sha256_file "${fixture}/coakka-tools/coakka-client/docker-demo/releases/1.3.1+2215b0f/coakka-client-docker-demo-v2-1.3.1-linux-x86_64.tar.gz")"
  cat >"${fixture}/artifacts/public-artifacts.tsv" <<EOF
# Public artifact manifest v1.
# Columns: status	label	relative_path	sha256
public	logger Native package	logger/native/releases/test/coakka-logger-native-test.tar.gz	${logger_sha}
public	runtime Native package	runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz	${runtime_sha}
public	coakka-runtime-inspect macos-aarch64	coakka-tools/coakka-runtime-inspect/releases/1.3.1+e664986/coakka-runtime-inspect-v2-1.3.1-macos-aarch64.tar.gz	${inspect_sha}
public	coakka-client Linux x86_64 CLI	coakka-tools/coakka-client/releases/1.3.1+2215b0f/coakka-client-v2-1.3.1-linux-x86_64.tar.gz	${cli_sha}
public	coakka-client Docker demo Linux x86_64	coakka-tools/coakka-client/docker-demo/releases/1.3.1+2215b0f/coakka-client-docker-demo-v2-1.3.1-linux-x86_64.tar.gz	${demo_sha}
EOF

  printf '%s\n' "${fixture}"
}

good_fixture="$(make_fixture good)"
expect_success "clean public manifest" env \
  COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE=1 \
  "${good_fixture}/scripts/verify-public-surface.sh"

bad_path_fixture="$(make_fixture bad-path)"
cat >>"${bad_path_fixture}/artifacts/public-artifacts.tsv" <<'EOF'
public	unsafe path	../outside.tar.gz	0000000000000000000000000000000000000000000000000000000000000000
EOF
expect_failure "path outside public manifest surface" env \
  COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE=1 \
  "${bad_path_fixture}/scripts/verify-public-surface.sh"
grep -Fq "unsafe artifact path" "${test_output}"

addon_without_manifest_fixture="$(make_fixture addon-without-manifest)"
addon_release_dir="${addon_without_manifest_fixture}/runtime-addons/example/native/releases/0.1.0+abcdef0"
mkdir -p "${addon_release_dir}"
printf 'addon archive\n' >"${addon_release_dir}/coakka-runtime-addon-example-native-0.1.0.tar.gz"
addon_sha="$(sha256_file "${addon_release_dir}/coakka-runtime-addon-example-native-0.1.0.tar.gz")"
cat >>"${addon_without_manifest_fixture}/artifacts/public-artifacts.tsv" <<EOF
public	runtime addon example	runtime-addons/example/native/releases/0.1.0+abcdef0/coakka-runtime-addon-example-native-0.1.0.tar.gz	${addon_sha}
EOF
expect_failure "runtime addon row without release manifest" env \
  COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE=1 \
  "${addon_without_manifest_fixture}/scripts/verify-public-surface.sh"
grep -Fq "runtime addon artifact has no release manifest" "${test_output}"

duplicate_path_fixture="$(make_fixture duplicate-path)"
runtime_sha="$(sha256_file "${duplicate_path_fixture}/runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz")"
cat >>"${duplicate_path_fixture}/artifacts/public-artifacts.tsv" <<EOF
public	runtime Native package duplicate	runtime/native/releases/0.1.0+63c346e/coakka-runtime-native-v2-0.1.0.tar.gz	${runtime_sha}
EOF
expect_failure "duplicate manifest path" env \
  COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE=1 \
  "${duplicate_path_fixture}/scripts/verify-public-surface.sh"
grep -Fq "duplicate artifact path" "${test_output}"

duplicate_label_fixture="$(make_fixture duplicate-label)"
logger_sha="$(sha256_file "${duplicate_label_fixture}/logger/native/releases/test/coakka-logger-native-test.tar.gz")"
cat >>"${duplicate_label_fixture}/artifacts/public-artifacts.tsv" <<EOF
public	logger Native package	logger/native/releases/test/coakka-logger-native-test-copy.tar.gz	${logger_sha}
EOF
expect_failure "duplicate manifest label" env \
  COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE=1 \
  "${duplicate_label_fixture}/scripts/verify-public-surface.sh"
grep -Fq "duplicate artifact label" "${test_output}"

bad_sha_fixture="$(make_fixture bad-sha)"
sed -i.bak 's/[0-9a-f]\{64\}$/ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff/' \
  "${bad_sha_fixture}/artifacts/public-artifacts.tsv"
expect_failure "manifest checksum mismatch" env \
  COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS=1 \
  COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE=1 \
  "${bad_sha_fixture}/scripts/verify-public-surface.sh"
grep -Fq "manifest sha256 mismatch" "${test_output}"

echo "[public-manifest-test] ok"
