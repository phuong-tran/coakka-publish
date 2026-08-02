#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
connector_root="${1:-${repo_root}/../coakkaJVMConnector}"
native_release="${COAKKA_NATIVE_RELEASE:-1.4.0+2cee86bf}"
native_manifest="${repo_root}/runtime/native/releases/${native_release}/manifest.json"

fail() {
  echo "[stage-runtime-connectors] $*" >&2
  exit 1
}

require_file() {
  [[ -f "$1" ]] || fail "missing required file: $1"
}

command -v jq >/dev/null 2>&1 || fail "jq is required"
require_file "${native_manifest}"
[[ -d "${connector_root}" ]] || fail "connector repo is missing: ${connector_root}"

native_version="$(jq -er '.version' "${native_manifest}")"
native_source="$(jq -er '.sourceSnapshot' "${native_manifest}")"
manifest_release="$(jq -er '.release' "${native_manifest}")"
[[ "${manifest_release}" == "${native_release}" ]] ||
  fail "native manifest release mismatch: ${manifest_release}"

connector_source="${COAKKA_CONNECTOR_SOURCE:-$(git -C "${connector_root}" rev-parse --short=7 HEAD)}"
connector_version="${COAKKA_CONNECTOR_VERSION:-${native_version}}"
release_directory="${native_release}-${connector_source}"
platforms_json="$(jq -c '.platforms' "${native_manifest}")"

tmp_parent="${repo_root}/.tmp"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/runtime-connectors.XXXXXX")"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

stage_lane() {
  local lane="$1"
  local artifact_version="$2"
  local artifact_path="$3"
  local docs_root="$4"
  local source_package="${5:-false}"
  local target="${repo_root}/runtime/${lane}/releases/${release_directory}"
  local staged="${tmp_root}/${lane}"
  local staged_sums="${tmp_root}/${lane}.SHA256SUMS"

  require_file "${artifact_path}"
  require_file "${docs_root}/README.md"
  [[ ! -e "${target}" ]] || fail "release already exists: ${target}"

  mkdir -p "${staged}"
  cp "${artifact_path}" "${staged}/"
  cp "${docs_root}/README.md" "${staged}/README.md"
  if [[ -f "${docs_root}/CONSUMING.md" ]]; then
    cp "${docs_root}/CONSUMING.md" "${staged}/CONSUMING.md"
  fi
  if [[ -f "${docs_root}/RELEASE_NOTES.md" ]]; then
    cp "${docs_root}/RELEASE_NOTES.md" "${staged}/RELEASE_NOTES.md"
  fi

  jq -n \
    --arg lane "${lane}" \
    --arg release_directory "${release_directory}" \
    --arg version "${artifact_version}" \
    --arg artifact_name "$(basename "${artifact_path}")" \
    --arg native_release "${native_release}" \
    --arg native_source "${native_source}" \
    --arg connector_source "${connector_source}" \
    --argjson platforms "${platforms_json}" \
    --argjson source_package "${source_package}" \
    '{
      schema_version: 1,
      product_lane: "runtime",
      language_lane: $lane,
      release_directory: $release_directory,
      version: $version,
      artifact: (
        {
          name: $artifact_name,
          bundled_native_package_version: $native_release,
          bundled_native_git_commit: $native_source,
          included_platforms: $platforms
        }
        + if $source_package then {source_package: true} else {} end
      ),
      connector_source_git_commit: $connector_source,
      payload_staging_git_commit: $connector_source
    }' >"${staged}/manifest.json"

  (
    cd "${staged}"
    find . -maxdepth 1 -type f ! -name SHA256SUMS -print |
      LC_ALL=C sort |
      sed 's#^./##' |
      xargs shasum -a 256 >"${staged_sums}"
    mv "${staged_sums}" SHA256SUMS
    shasum -a 256 -c SHA256SUMS >/dev/null
  )

  mkdir -p "$(dirname "${target}")"
  mv "${staged}" "${target}"
  echo "[stage-runtime-connectors] staged runtime/${lane}/releases/${release_directory}"
}

jvm_version="${connector_version}-g${native_source}-${connector_source}"
jvm_dist="${connector_root}/v2/jvm/build/dist/coakka-jvm-native-runtime-v2"

stage_lane \
  jvm "${jvm_version}" \
  "${jvm_dist}/coakka-jvm-native-runtime-v2-${jvm_version}.jar" \
  "${jvm_dist}"

stage_lane node "${connector_version}" \
  "${connector_root}/node/coakka-v2-connector-node-${connector_version}.tgz" \
  "${connector_root}/node"
stage_lane bun "${connector_version}" \
  "${connector_root}/bun/coakka-v2-connector-bun-${connector_version}.tgz" \
  "${connector_root}/bun"
stage_lane electron "${connector_version}" \
  "${connector_root}/electron/coakka-v2-connector-electron-${connector_version}.tgz" \
  "${connector_root}/electron"
stage_lane python "${connector_version}" \
  "${connector_root}/python/build/wheelhouse/coakka_v2_connector-${connector_version}-py3-none-any.whl" \
  "${connector_root}/python"
stage_lane go "${connector_version}" \
  "${connector_root}/go/coakka-v2-connector-go-${connector_version}.tar.gz" \
  "${connector_root}/go"
stage_lane csharp "${connector_version}" \
  "${connector_root}/csharp/build/nupkg/CoAkka.Runtime.${connector_version}.nupkg" \
  "${connector_root}/csharp"
stage_lane rust "${connector_version}" \
  "${connector_root}/rust/coakka-runtime-rs-${connector_version}.tar.gz" \
  "${connector_root}/rust" true
stage_lane swift "${connector_version}" \
  "${connector_root}/swift/coakka-runtime-swift-${connector_version}.tar.gz" \
  "${connector_root}/swift" true
stage_lane tauri "${connector_version}-source" \
  "${connector_root}/tauri-intents/coakka-runtime-tauri-intents-${connector_version}-source.tar.gz" \
  "${connector_root}/tauri-intents" true
stage_lane mojo "${connector_version}-source" \
  "${connector_root}/mojo/coakka-runtime-mojo-${connector_version}-source.tar.gz" \
  "${connector_root}/mojo" true
stage_lane zig "${connector_version}-source" \
  "${connector_root}/zig/coakka-runtime-zig-${connector_version}-source.tar.gz" \
  "${connector_root}/zig" true

echo "[stage-runtime-connectors] release=${release_directory} platforms=${platforms_json}"
