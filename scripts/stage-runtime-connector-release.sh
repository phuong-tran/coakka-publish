#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
connector_root="${1:-${repo_root}/../coakkaJVMConnector}"
native_release="${COAKKA_NATIVE_RELEASE:-1.4.1+9e02a51d}"
native_manifest="${repo_root}/runtime/native/releases/${native_release}/manifest.json"
lanes_spec="${COAKKA_CONNECTOR_LANES-jvm node bun electron python go csharp rust swift tauri mojo zig}"
supported_lanes=(jvm node bun electron python go csharp rust swift tauri mojo zig)
release_lanes=()

fail() {
  echo "[stage-runtime-connectors] $*" >&2
  exit 1
}

require_file() {
  [[ -f "$1" ]] || fail "missing required file: $1"
}

parse_release_lanes() {
  local normalized lane supported existing
  normalized="${lanes_spec//,/ }"
  for lane in ${normalized}; do
    supported=0
    for existing in "${supported_lanes[@]}"; do
      if [[ "${lane}" == "${existing}" ]]; then
        supported=1
        break
      fi
    done
    [[ "${supported}" == "1" ]] || fail "unsupported connector lane: ${lane}"
    if [[ "${#release_lanes[@]}" -gt 0 ]]; then
      for existing in "${release_lanes[@]}"; do
        [[ "${lane}" != "${existing}" ]] || fail "duplicate connector lane: ${lane}"
      done
    fi
    release_lanes+=("${lane}")
  done
  [[ "${#release_lanes[@]}" -gt 0 ]] || fail "COAKKA_CONNECTOR_LANES must select at least one lane"
}

lane_selected() {
  local wanted="$1" lane
  for lane in "${release_lanes[@]}"; do
    [[ "${lane}" == "${wanted}" ]] && return 0
  done
  return 1
}

artifact_label_for_lane() {
  case "$1" in
    jvm) printf '%s\n' "runtime JVM jar" ;;
    node) printf '%s\n' "runtime Node package" ;;
    bun) printf '%s\n' "runtime Bun package" ;;
    electron) printf '%s\n' "runtime Electron package" ;;
    python) printf '%s\n' "runtime Python wheel" ;;
    go) printf '%s\n' "runtime Go package" ;;
    csharp) printf '%s\n' "runtime C# package" ;;
    rust) printf '%s\n' "runtime Rust package" ;;
    swift) printf '%s\n' "runtime Swift package" ;;
    tauri) printf '%s\n' "runtime Tauri source package" ;;
    mojo) printf '%s\n' "runtime Mojo source package" ;;
    zig) printf '%s\n' "runtime Zig source package" ;;
    *) fail "missing artifact label for connector lane: $1" ;;
  esac
}

refresh_artifact_ledger_row() {
  local lane="$1"
  local target_artifact="$2"
  local ledger="${repo_root}/artifacts/public-artifacts.tsv"
  local label relative_path digest
  label="$(artifact_label_for_lane "${lane}")"
  relative_path="${target_artifact#${repo_root}/}"
  [[ "${relative_path}" != "${target_artifact}" ]] || fail "artifact is outside publish root: ${target_artifact}"
  digest="$(shasum -a 256 "${target_artifact}" | awk '{print $1}')"

  python3 - "${ledger}" "${label}" "${relative_path}" "${digest}" <<'PY'
import csv
import os
import sys
import tempfile

ledger, label, relative_path, digest = sys.argv[1:]
with open(ledger, encoding="utf-8", newline="") as stream:
    lines = stream.readlines()

replacement = f"public\t{label}\t{relative_path}\t{digest}\n"
matched = 0
output = []
for line in lines:
    if line.startswith("#") or not line.strip():
        output.append(line)
        continue
    fields = next(csv.reader([line], delimiter="\t"))
    if len(fields) == 4 and fields[1] == label:
        matched += 1
        if matched == 1:
            output.append(replacement)
        continue
    output.append(line)

if matched > 1:
    raise SystemExit(f"duplicate artifact ledger label: {label}")
if matched == 0:
    output.append(replacement)

directory = os.path.dirname(ledger)
fd, temporary = tempfile.mkstemp(prefix="public-artifacts.", dir=directory, text=True)
try:
    with os.fdopen(fd, "w", encoding="utf-8", newline="") as stream:
        stream.writelines(output)
    os.replace(temporary, ledger)
except BaseException:
    try:
        os.unlink(temporary)
    except FileNotFoundError:
        pass
    raise
PY
}

command -v jq >/dev/null 2>&1 || fail "jq is required"
require_file "${native_manifest}"
[[ -d "${connector_root}" ]] || fail "connector repo is missing: ${connector_root}"
parse_release_lanes

native_version="$(jq -er '.version' "${native_manifest}")"
native_source="$(jq -er '.sourceSnapshot' "${native_manifest}")"
manifest_release="$(jq -er '.release' "${native_manifest}")"
[[ "${manifest_release}" == "${native_release}" ]] ||
  fail "native manifest release mismatch: ${manifest_release}"

connector_source="${COAKKA_CONNECTOR_SOURCE:-$(git -C "${connector_root}" rev-parse --short=7 HEAD)}"
payload_staging_source="${COAKKA_PAYLOAD_STAGING_SOURCE:-$(git -C "${connector_root}" rev-parse --short=7 HEAD)}"
connector_version="${COAKKA_CONNECTOR_VERSION:-${native_version}}"
release_directory="${native_release}-${connector_source}"
platforms_json="$(jq -c '.platforms' "${native_manifest}")"

git -C "${connector_root}" rev-parse --verify "${connector_source}^{commit}" >/dev/null 2>&1 ||
  fail "COAKKA_CONNECTOR_SOURCE does not resolve to a connector commit: ${connector_source}"
git -C "${connector_root}" rev-parse --verify "${payload_staging_source}^{commit}" >/dev/null 2>&1 ||
  fail "COAKKA_PAYLOAD_STAGING_SOURCE does not resolve to a connector commit: ${payload_staging_source}"

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
  while IFS= read -r package_doc; do
    cp "${package_doc}" "${staged}/"
  done < <(find "${docs_root}" -maxdepth 1 -type f -name '*.md' | LC_ALL=C sort)
  if [[ -f "${docs_root}/LICENSE" ]]; then
    cp "${docs_root}/LICENSE" "${staged}/LICENSE"
  fi
  case "${lane}" in
    jvm)
      cp "${connector_root}/v2/jvm/RELEASE.md" "${staged}/RELEASE.md"
      mkdir -p "${staged}/consumer-smoke"
      cp "${connector_root}/v2/jvm/consumer-smoke/README.md" \
        "${staged}/consumer-smoke/README.md"
      ;;
    swift)
      mkdir -p "${staged}/Sources/CoAkkaRuntimeC/include"
      cp "${connector_root}/swift/Sources/CoAkkaRuntimeC/include/coakka_runtime_bridge.h" \
        "${staged}/Sources/CoAkkaRuntimeC/include/coakka_runtime_bridge.h"
      ;;
  esac

  jq -n \
    --arg lane "${lane}" \
    --arg release_directory "${release_directory}" \
    --arg version "${artifact_version}" \
    --arg artifact_name "$(basename "${artifact_path}")" \
    --arg native_release "${native_release}" \
    --arg native_source "${native_source}" \
    --arg connector_source "${connector_source}" \
    --arg payload_staging_source "${payload_staging_source}" \
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
      payload_staging_git_commit: $payload_staging_source
    }' >"${staged}/manifest.json"

  (
    cd "${staged}"
    find . -type f ! -name SHA256SUMS -print |
      LC_ALL=C sort |
      sed 's#^./##' |
      xargs shasum -a 256 >"${staged_sums}"
    mv "${staged_sums}" SHA256SUMS
    shasum -a 256 -c SHA256SUMS >/dev/null
  )

  mkdir -p "$(dirname "${target}")"
  mv "${staged}" "${target}"
  refresh_artifact_ledger_row "${lane}" "${target}/$(basename "${artifact_path}")"
  echo "[stage-runtime-connectors] staged runtime/${lane}/releases/${release_directory}"
}

jvm_version="${connector_version}-g${native_source}-${connector_source}"
jvm_dist="${connector_root}/v2/jvm/build/dist/coakka-jvm-native-runtime-v2"

if lane_selected jvm; then
  stage_lane \
    jvm "${jvm_version}" \
    "${jvm_dist}/coakka-jvm-native-runtime-v2-${jvm_version}.jar" \
    "${jvm_dist}"
fi

lane_selected node && stage_lane node "${connector_version}" \
  "${connector_root}/node/coakka-v2-connector-node-${connector_version}.tgz" \
  "${connector_root}/node"
lane_selected bun && stage_lane bun "${connector_version}" \
  "${connector_root}/bun/coakka-v2-connector-bun-${connector_version}.tgz" \
  "${connector_root}/bun"
lane_selected electron && stage_lane electron "${connector_version}" \
  "${connector_root}/electron/coakka-v2-connector-electron-${connector_version}.tgz" \
  "${connector_root}/electron"
lane_selected python && stage_lane python "${connector_version}" \
  "${connector_root}/python/build/wheelhouse/coakka_v2_connector-${connector_version}-py3-none-any.whl" \
  "${connector_root}/python"
lane_selected go && stage_lane go "${connector_version}" \
  "${connector_root}/go/coakka-v2-connector-go-${connector_version}.tar.gz" \
  "${connector_root}/go"
lane_selected csharp && stage_lane csharp "${connector_version}" \
  "${connector_root}/csharp/build/nupkg/CoAkka.Runtime.${connector_version}.nupkg" \
  "${connector_root}/csharp"
lane_selected rust && stage_lane rust "${connector_version}" \
  "${connector_root}/rust/coakka-runtime-rs-${connector_version}.tar.gz" \
  "${connector_root}/rust" true
lane_selected swift && stage_lane swift "${connector_version}" \
  "${connector_root}/swift/coakka-runtime-swift-${connector_version}.tar.gz" \
  "${connector_root}/swift" true
lane_selected tauri && stage_lane tauri "${connector_version}-source" \
  "${connector_root}/tauri-intents/coakka-runtime-tauri-intents-${connector_version}-source.tar.gz" \
  "${connector_root}/tauri-intents" true
lane_selected mojo && stage_lane mojo "${connector_version}-source" \
  "${connector_root}/mojo/coakka-runtime-mojo-${connector_version}-source.tar.gz" \
  "${connector_root}/mojo" true
lane_selected zig && stage_lane zig "${connector_version}-source" \
  "${connector_root}/zig/coakka-runtime-zig-${connector_version}-source.tar.gz" \
  "${connector_root}/zig" true

echo "[stage-runtime-connectors] release=${release_directory} lanes=${release_lanes[*]} platforms=${platforms_json}"
