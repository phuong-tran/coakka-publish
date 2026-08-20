#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tmp_parent="${COAKKA_PUBLIC_TMP_ROOT:-${repo_root}/.tmp}"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/coakka-npm-package-manager.XXXXXX")"
test_output="${tmp_root}/last-command.out"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

expect_success() {
  local label="$1"
  shift
  if ! "$@" >"${test_output}" 2>&1; then
    echo "[npm-package-manager-test] expected success: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

expect_failure() {
  local label="$1"
  shift
  if "$@" >"${test_output}" 2>&1; then
    echo "[npm-package-manager-test] expected failure: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

make_package() {
  local target="$1"
  local product="$2"
  local role="$3"
  local package_name="$4"
  local dependency_name="${5:-}"
  local dependency_spec="${6:-}"
  local private_value="${7:-false}"
  local license_value="${8:-SEE LICENSE IN PACKAGE-LICENSE.md}"
  local include_proto="${9:-false}"
  local include_install="${10:-false}"
  local private_repo_metadata="${11:-false}"
  local include_unversioned_alias="${12:-false}"
  local packaged_platforms="${13:-linux-aarch64 linux-x86_64 macos-aarch64 windows-aarch64 windows-x86_64}"
  local include_exports="${14:-true}"
  local root="${tmp_root}/pkg-root/package"
  local native_generation="1.3.1+abcdef0"

  rm -rf "${tmp_root}/pkg-root"
  mkdir -p "${root}/dist"

  local dependency_block=""
  if [[ -n "${dependency_name}" ]]; then
    dependency_block=",
  \"dependencies\": {
    \"${dependency_name}\": \"${dependency_spec}\"
  }"
  fi

  local private_block=""
  if [[ "${private_value}" == "true" ]]; then
    private_block=",
  \"private\": true"
  fi

  local install_block=""
  if [[ "${include_install}" == "true" ]]; then
    install_block=",
    \"install\": \"node scripts/install.js\""
  fi

  local repository_url="git+https://github.com/phuong-tran/coakka-publish.git"
  local bugs_url="https://github.com/phuong-tran/coakka-samples/issues"
  if [[ "${private_repo_metadata}" == "true" ]]; then
    repository_url="git+https://github.com/phuong-tran/coakkaJVMConnector.git"
    bugs_url="https://github.com/phuong-tran/coakkaJVMConnector/issues"
  fi

  cat >"${root}/package.json" <<EOF
{
  "name": "${package_name}",
  "version": "1.3.1",
  "license": "${license_value}",
  "repository": {
    "type": "git",
    "url": "${repository_url}"
  },
  "bugs": {
    "url": "${bugs_url}"
  },
  "type": "module",
  "main": "./dist/index.js",
  "types": "./dist/index.d.ts",
  "scripts": {
    "verify": "node --check dist/index.js"${install_block}
  }${private_block}${dependency_block}
}
EOF
  printf 'export {};\n' >"${root}/dist/index.js"
  printf 'export {};\n' >"${root}/dist/index.d.ts"
  printf 'Apache License fixture\n' >"${root}/LICENSE"
  printf '# Native License fixture\n' >"${root}/NATIVE-LICENSE.md"
  printf '# Package License fixture\n' >"${root}/PACKAGE-LICENSE.md"
  printf 'Notice fixture\n' >"${root}/NOTICE"

  if [[ "${include_proto}" == "true" ]]; then
    mkdir -p "${root}/proto/coakka/v2"
    printf 'syntax = "proto3";\n' >"${root}/proto/coakka/v2/transport.proto"
  fi

  if [[ "${role}" != "electron" ]]; then
    local lib_base
    case "${product}" in
      runtime) lib_base="libcoakka_runtime_v2" ;;
      logger) lib_base="libcoakka_logger_core" ;;
      *) echo "unsupported product: ${product}" >&2; exit 1 ;;
    esac

    local platform extension
    for platform in ${packaged_platforms}; do
      case "${platform}" in
        macos-*) extension="dylib" ;;
        windows-*) extension="dll" ;;
        *) extension="so" ;;
      esac
      mkdir -p "${root}/native/${platform}"
      if [[ "${platform}" == "macos-aarch64" ]]; then
        # Minimal thin ARM64 Mach-O with one LC_BUILD_VERSION command at macOS 13.0.
        printf '\xcf\xfa\xed\xfe\x0c\x00\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x01\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x32\x00\x00\x00\x18\x00\x00\x00\x01\x00\x00\x00\x00\x00\x0d\x00\x00\x00\x0d\x00\x00\x00\x00\x00' \
          >"${root}/native/${platform}/${lib_base}-${native_generation}.${extension}"
      else
        printf 'native placeholder\n' >"${root}/native/${platform}/${lib_base}-${native_generation}.${extension}"
      fi
      if [[ "${include_unversioned_alias}" == "true" ]]; then
        printf 'native placeholder\n' >"${root}/native/${platform}/${lib_base}.${extension}"
      fi
    done
  fi

  if [[ "${include_exports}" == "true" ]]; then
    python3 - "${root}/package.json" "${role}" <<'PY'
import json
import sys

path = sys.argv[1]
role = sys.argv[2]
with open(path, "r", encoding="utf-8") as fh:
    package = json.load(fh)
package["exports"] = {
    ".": {"types": "./dist/index.d.ts", "import": "./dist/index.js"},
}
if role == "electron":
    package["exports"]["./preload"] = {
        "types": "./dist/index.d.ts",
        "import": "./dist/index.js",
        "require": "./dist/index.js",
    }
with open(path, "w", encoding="utf-8") as fh:
    json.dump(package, fh, indent=2)
    fh.write("\n")
PY
  fi

  COPYFILE_DISABLE=1 tar -C "${tmp_root}/pkg-root" -czf "${target}" package
}

verify_fixture() {
  local artifact="$1"
  local product="$2"
  local role="$3"
  local package_name="$4"
  shift 4
  "${repo_root}/scripts/verify-npm-package-manager-artifact.py" \
    --artifact "${artifact}" \
    --product "${product}" \
    --role "${role}" \
    --package-name "${package_name}" \
    --expected-native-generation "1.3.1+abcdef0" \
    --require-public-metadata \
    "$@"
}

good_node="${tmp_root}/coakka-v2-connector-node-1.3.1.tgz"
make_package "${good_node}" runtime node coakka-v2-connector-node
expect_success "clean Node package" verify_fixture "${good_node}" runtime node coakka-v2-connector-node

internal_codec_node="${tmp_root}/coakka-v2-connector-node-internal-codec.tgz"
make_package "${internal_codec_node}" runtime node coakka-v2-connector-node
cat >"${tmp_root}/pkg-root/package/dist/internal-transport-codec.js" <<'EOF'
const WIRE_VARINT = 0;
function encodeVarint(value) {
  return value;
}
export function encodeEnvelope(value) {
  return encodeVarint(value + WIRE_VARINT);
}
export function decodeEnvelope(bytes) {
  return bytes;
}
export function decodeDeadletter(bytes) {
  return bytes;
}
EOF
COPYFILE_DISABLE=1 tar -C "${tmp_root}/pkg-root" -czf "${internal_codec_node}" package
expect_success \
  "internal transport codec behind exports" \
  verify_fixture \
  "${internal_codec_node}" \
  runtime \
  node \
  coakka-v2-connector-node

missing_exports="${tmp_root}/coakka-v2-connector-node-missing-exports.tgz"
make_package \
  "${missing_exports}" \
  runtime \
  node \
  coakka-v2-connector-node \
  "" \
  "" \
  false \
  "SEE LICENSE IN PACKAGE-LICENSE.md" \
  false \
  false \
  false \
  false \
  "linux-aarch64 linux-x86_64 macos-aarch64 windows-aarch64 windows-x86_64" \
  false
expect_failure \
  "missing explicit exports" \
  verify_fixture \
  "${missing_exports}" \
  runtime \
  node \
  coakka-v2-connector-node
grep -Fq "must declare explicit package exports" "${test_output}"

exported_internal_codec="${tmp_root}/coakka-v2-connector-node-exported-internal-codec.tgz"
make_package "${exported_internal_codec}" runtime node coakka-v2-connector-node
python3 - "${tmp_root}/pkg-root/package/package.json" <<'PY'
import json
import sys

path = sys.argv[1]
with open(path, "r", encoding="utf-8") as fh:
    package = json.load(fh)
package["exports"]["./internal-transport-codec"] = "./dist/internal-transport-codec.js"
with open(path, "w", encoding="utf-8") as fh:
    json.dump(package, fh, indent=2)
    fh.write("\n")
PY
COPYFILE_DISABLE=1 tar -C "${tmp_root}/pkg-root" -czf "${exported_internal_codec}" package
expect_failure \
  "exported internal transport codec" \
  verify_fixture \
  "${exported_internal_codec}" \
  runtime \
  node \
  coakka-v2-connector-node
grep -Fq "exports expose internal transport path" "${test_output}"

release_matrix_node="${tmp_root}/coakka-v2-connector-node-release-matrix.tgz"
make_package \
  "${release_matrix_node}" \
  runtime \
  node \
  coakka-v2-connector-node \
  "" \
  "" \
  false \
  "SEE LICENSE IN PACKAGE-LICENSE.md" \
  false \
  false \
  false \
  false \
  "linux-aarch64 macos-aarch64 windows-x86_64"
expect_success \
  "exact release platform matrix" \
  verify_fixture \
  "${release_matrix_node}" \
  runtime \
  node \
  coakka-v2-connector-node \
  --expected-platform linux-aarch64 \
  --expected-platform macos-aarch64 \
  --expected-platform windows-x86_64

expect_failure \
  "native outside release platform matrix" \
  verify_fixture \
  "${good_node}" \
  runtime \
  node \
  coakka-v2-connector-node \
  --expected-platform linux-aarch64 \
  --expected-platform macos-aarch64 \
  --expected-platform windows-x86_64
grep -Fq "outside the release matrix" "${test_output}"

expect_failure \
  "missing release platform native" \
  verify_fixture \
  "${release_matrix_node}" \
  runtime \
  node \
  coakka-v2-connector-node \
  --expected-platform linux-aarch64 \
  --expected-platform linux-x86_64 \
  --expected-platform macos-aarch64 \
  --expected-platform windows-x86_64
grep -Fq "missing bundled runtime native libraries for: linux-x86_64" "${test_output}"

duplicate_native_alias="${tmp_root}/coakka-v2-connector-node-duplicate-native.tgz"
make_package \
  "${duplicate_native_alias}" \
  runtime \
  node \
  coakka-v2-connector-node \
  "" \
  "" \
  false \
  "SEE LICENSE IN PACKAGE-LICENSE.md" \
  false \
  false \
  false \
  true
expect_failure "duplicate native alias" verify_fixture "${duplicate_native_alias}" runtime node coakka-v2-connector-node
grep -Fq "must include exactly one runtime native" "${test_output}"

good_electron="${tmp_root}/coakka-v2-connector-electron-1.3.1.tgz"
make_package "${good_electron}" runtime electron coakka-v2-connector-electron coakka-v2-connector-node 1.3.1
expect_success \
  "clean Electron package" \
  verify_fixture \
  "${good_electron}" \
  runtime \
  electron \
  coakka-v2-connector-electron \
  --expected-internal-dependency coakka-v2-connector-node

private_logger="${tmp_root}/coakka-logger-node-private.tgz"
make_package "${private_logger}" logger node coakka-logger-node "" "" true
expect_failure "private package" verify_fixture "${private_logger}" logger node coakka-logger-node
grep -Fq "private=true" "${test_output}"

missing_license="${tmp_root}/coakka-logger-bun-missing-license.tgz"
make_package "${missing_license}" logger bun coakka-logger-bun "" "" false UNLICENSED
expect_failure "license mismatch" verify_fixture "${missing_license}" logger bun coakka-logger-bun
grep -Fq "package license must be" "${test_output}"

missing_package_license="${tmp_root}/coakka-logger-bun-missing-package-license.tgz"
make_package "${missing_package_license}" logger bun coakka-logger-bun
rm "${tmp_root}/pkg-root/package/PACKAGE-LICENSE.md"
COPYFILE_DISABLE=1 tar -C "${tmp_root}/pkg-root" -czf "${missing_package_license}" package
expect_failure \
  "missing package license file" \
  verify_fixture \
  "${missing_package_license}" \
  logger \
  bun \
  coakka-logger-bun
grep -Fq "missing required license members" "${test_output}"

protobuf_dep="${tmp_root}/coakka-v2-connector-node-protobuf.tgz"
make_package "${protobuf_dep}" runtime node coakka-v2-connector-node protobufjs '^8.0.3'
expect_failure "forbidden implementation dependency" verify_fixture "${protobuf_dep}" runtime node coakka-v2-connector-node
grep -Fq "forbidden native or implementation dependency" "${test_output}"

multi_forbidden_dep="${tmp_root}/coakka-v2-connector-node-multi-forbidden.tgz"
make_package "${multi_forbidden_dep}" runtime node coakka-v2-connector-node koffi '^2.16.1'
python3 - "${tmp_root}/pkg-root/package/package.json" <<'PY'
import json
import sys

path = sys.argv[1]
with open(path, "r", encoding="utf-8") as fh:
    package = json.load(fh)
package["dependencies"]["protobufjs"] = "^8.0.3"
with open(path, "w", encoding="utf-8") as fh:
    json.dump(package, fh, indent=2)
    fh.write("\n")
PY
COPYFILE_DISABLE=1 tar -C "${tmp_root}/pkg-root" -czf "${multi_forbidden_dep}" package
expect_failure "multiple forbidden dependencies" verify_fixture "${multi_forbidden_dep}" runtime node coakka-v2-connector-node
grep -Fq "koffi" "${test_output}"
grep -Fq "protobufjs" "${test_output}"

raw_dependency="${tmp_root}/coakka-v2-connector-electron-raw-dependency.tgz"
make_package \
  "${raw_dependency}" \
  runtime \
  electron \
  coakka-v2-connector-electron \
  coakka-v2-connector-node \
  "https://raw.githubusercontent.com/phuong-tran/coakka-publish/main/runtime/node/releases/1.3.1/coakka-v2-connector-node-1.3.1.tgz"
expect_failure \
  "raw tarball dependency" \
  verify_fixture \
  "${raw_dependency}" \
  runtime \
  electron \
  coakka-v2-connector-electron \
  --expected-internal-dependency coakka-v2-connector-node
grep -Fq "must resolve through registry metadata" "${test_output}"

proto_leak="${tmp_root}/coakka-v2-connector-node-proto.tgz"
make_package "${proto_leak}" runtime node coakka-v2-connector-node "" "" false "SEE LICENSE IN PACKAGE-LICENSE.md" true
expect_failure "proto leak" verify_fixture "${proto_leak}" runtime node coakka-v2-connector-node
grep -Fq "protobuf schema leaked" "${test_output}"

text_wire_leak="${tmp_root}/coakka-v2-connector-node-wire-text.tgz"
make_package "${text_wire_leak}" runtime node coakka-v2-connector-node
cat >"${tmp_root}/pkg-root/package/dist/protobuf.js" <<'EOF'
const WIRE_VARINT = 0;
function encodeVarint(value) {
  return value;
}
export function decodeEnvelope(bytes) {
  return bytes;
}
EOF
COPYFILE_DISABLE=1 tar -C "${tmp_root}/pkg-root" -czf "${text_wire_leak}" package
expect_failure "text wire codec leak" verify_fixture "${text_wire_leak}" runtime node coakka-v2-connector-node
grep -Eq "public transport (codec member|framing implementation)" "${test_output}"

install_script="${tmp_root}/coakka-v2-connector-node-install.tgz"
make_package "${install_script}" runtime node coakka-v2-connector-node "" "" false "SEE LICENSE IN PACKAGE-LICENSE.md" false true
expect_failure "install lifecycle script" verify_fixture "${install_script}" runtime node coakka-v2-connector-node
grep -Fq "lifecycle script" "${test_output}"

private_repo_metadata="${tmp_root}/coakka-v2-connector-node-private-repo.tgz"
make_package "${private_repo_metadata}" runtime node coakka-v2-connector-node "" "" false "SEE LICENSE IN PACKAGE-LICENSE.md" false false true
expect_failure "private repo metadata" verify_fixture "${private_repo_metadata}" runtime node coakka-v2-connector-node
grep -Fq "private source repo marker leaked into npm metadata" "${test_output}"

echo "[npm-package-manager-test] ok"
