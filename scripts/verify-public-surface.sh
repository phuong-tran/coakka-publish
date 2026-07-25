#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
default_scanner="${repo_root}/scripts/scan-public-surface.sh"

fail() {
  echo "[verify-public-surface] $*" >&2
  exit 1
}

require_file() {
  local path="$1"
  if [[ ! -f "${repo_root}/${path}" ]]; then
    fail "missing required file: ${path}"
  fi
}

verify_sha256_file() {
  local dir="$1"
  local sums="${repo_root}/${dir}/SHA256SUMS"
  if [[ ! -f "${sums}" ]]; then
    echo "[verify-public-surface] missing checksum file: ${dir}/SHA256SUMS" >&2
    exit 1
  fi
  (cd "${repo_root}/${dir}" && shasum -a 256 -c SHA256SUMS >/dev/null)
}

file_digest() {
  local algorithm="$1"
  local path="$2"
  case "${algorithm}" in
    sha256)
      shasum -a 256 "${path}" | awk '{print $1}'
      ;;
    sha1)
      shasum -a 1 "${path}" | awk '{print $1}'
      ;;
    md5)
      if command -v md5sum >/dev/null 2>&1; then
        md5sum "${path}" | awk '{print $1}'
      elif command -v md5 >/dev/null 2>&1; then
        md5 -q "${path}"
      else
        fail "md5sum or md5 is required for Maven md5 sidecars"
      fi
      ;;
    *)
      fail "unsupported digest algorithm: ${algorithm}"
      ;;
  esac
}

verify_digest_sidecar() {
  local algorithm="$1"
  local file="$2"
  local sidecar="${file}.${algorithm}"
  local expected actual
  [[ -f "${sidecar}" ]] || return 0
  expected="$(awk '{print $1}' "${sidecar}")"
  actual="$(file_digest "${algorithm}" "${file}")"
  if [[ "${actual}" != "${expected}" ]]; then
    fail "${algorithm} mismatch for ${file#"${repo_root}"/}"
  fi
}

verify_maven_sidecars() {
  local file
  while IFS= read -r -d '' file; do
    case "${file}" in
      *.md5|*.sha1|*.sha256) continue ;;
    esac
    verify_digest_sidecar sha256 "${file}"
    verify_digest_sidecar sha1 "${file}"
    verify_digest_sidecar md5 "${file}"
  done < <(find "${repo_root}/maven" -type f -print0)
}

public_manifest_path_for_label() {
  local label_to_find="$1"
  local manifest="${repo_root}/artifacts/public-artifacts.tsv"
  local matches=()
  local status label relative_path expected_sha extra

  while IFS=$'\t' read -r status label relative_path expected_sha extra || [[ -n "${status:-}" ]]; do
    [[ -z "${status:-}" || "${status}" == \#* ]] && continue
    if [[ "${status}" == "public" && "${label}" == "${label_to_find}" ]]; then
      matches+=("${relative_path}")
    fi
  done <"${manifest}"

  if [[ "${#matches[@]}" -ne 1 ]]; then
    fail "expected exactly one public manifest row for ${label_to_find}, found ${#matches[@]}"
  fi
  printf '%s\n' "${matches[0]}"
}

maven_metadata_latest() {
  local metadata="$1"
  sed -n 's:.*<latest>\(.*\)</latest>.*:\1:p' "${repo_root}/${metadata}" |
    sed -n '1p'
}

jar_manifest_attribute() {
  local jar_path="$1"
  local attribute="$2"
  unzip -p "${jar_path}" META-INF/MANIFEST.MF |
    awk -F': ' -v attribute="${attribute}" '
      $1 == attribute {
        gsub(/\r$/, "", $2)
        print $2
        exit
      }
    '
}

pom_runtime_jvm_dependency_version() {
  local pom_path="$1"
  awk '
    /<artifactId>coakka-jvm-native-runtime-v2<\/artifactId>/ {
      in_runtime_dependency = 1
      next
    }
    in_runtime_dependency && /<version>/ {
      gsub(/.*<version>/, "")
      gsub(/<\/version>.*/, "")
      print
      exit
    }
  ' "${pom_path}"
}

verify_framework_adapter_runtime_dependency() {
  local label="$1"
  local expected_runtime_version="$2"
  local relative_path jar_path pom_path manifest_version pom_version

  relative_path="$(public_manifest_path_for_label "${label}")"
  jar_path="${repo_root}/${relative_path}"
  pom_path="${jar_path%.jar}.pom"
  [[ -f "${jar_path}" ]] || fail "${label} jar is missing: ${relative_path}"
  [[ -f "${pom_path}" ]] || fail "${label} pom is missing: ${pom_path#"${repo_root}"/}"

  manifest_version="$(jar_manifest_attribute "${jar_path}" "Coakka-Runtime-Jvm-Dependency")"
  if [[ "${manifest_version}" != "${expected_runtime_version}" ]]; then
    fail "${relative_path} declares Coakka-Runtime-Jvm-Dependency=${manifest_version}; expected ${expected_runtime_version}"
  fi

  pom_version="$(pom_runtime_jvm_dependency_version "${pom_path}")"
  if [[ "${pom_version}" != "${expected_runtime_version}" ]]; then
    fail "${pom_path#"${repo_root}"/} depends on runtime JVM ${pom_version}; expected ${expected_runtime_version}"
  fi
}

verify_framework_adapter_dependencies() {
  local runtime_jvm_latest
  runtime_jvm_latest="$(maven_metadata_latest "maven/coakka/v2/coakka-jvm-native-runtime-v2/maven-metadata.xml")"
  [[ -n "${runtime_jvm_latest}" ]] || fail "could not read runtime JVM latest Maven version"

  verify_framework_adapter_runtime_dependency "Spring Boot starter Maven jar" "${runtime_jvm_latest}"
  verify_framework_adapter_runtime_dependency "Quarkus extension Maven jar" "${runtime_jvm_latest}"
}

verify_public_artifact_manifest() {
  local manifest="${repo_root}/artifacts/public-artifacts.tsv"
  local line_no=0
  local public_rows=0
  local seen_paths=$'\n'
  local seen_labels=$'\n'
  local status label relative_path expected_sha extra actual_sha

  while IFS=$'\t' read -r status label relative_path expected_sha extra || [[ -n "${status:-}" ]]; do
    line_no=$((line_no + 1))
    [[ -z "${status:-}" || "${status}" == \#* ]] && continue

    if [[ -n "${extra:-}" || -z "${label:-}" || -z "${relative_path:-}" || -z "${expected_sha:-}" ]]; then
      fail "invalid artifacts/public-artifacts.tsv row ${line_no}"
    fi
    if [[ "${status}" != "public" ]]; then
      fail "unsupported artifact status '${status}' in artifacts/public-artifacts.tsv row ${line_no}"
    fi
    if [[ "${relative_path}" == /* || "${relative_path}" == *".."* ]]; then
      fail "unsafe artifact path in manifest row ${line_no}: ${relative_path}"
    fi
    case "${relative_path}" in
      logger/*/releases/*|runtime/*/releases/*|runtime-inspect/*/releases/*|cli/releases/*|demo/coakka-client/releases/*|maven/coakka/*/*/*/*.jar)
        ;;
      *)
        fail "artifact path is outside the current public manifest surface in row ${line_no}: ${relative_path}"
        ;;
    esac
    if [[ "${seen_paths}" == *$'\n'"${relative_path}"$'\n'* ]]; then
      fail "duplicate artifact path in manifest row ${line_no}: ${relative_path}"
    fi
    if [[ "${seen_labels}" == *$'\n'"${label}"$'\n'* ]]; then
      fail "duplicate artifact label in manifest row ${line_no}: ${label}"
    fi
    if [[ ! "${expected_sha}" =~ ^[0-9a-f]{64}$ ]]; then
      fail "invalid sha256 in artifacts/public-artifacts.tsv row ${line_no}"
    fi
    if [[ ! -f "${repo_root}/${relative_path}" ]]; then
      fail "manifest artifact is missing: ${relative_path}"
    fi

    actual_sha="$(file_digest sha256 "${repo_root}/${relative_path}")"
    if [[ "${actual_sha}" != "${expected_sha}" ]]; then
      fail "manifest sha256 mismatch for ${relative_path}"
    fi
    seen_paths+="${relative_path}"$'\n'
    seen_labels+="${label}"$'\n'
    public_rows=$((public_rows + 1))
  done <"${manifest}"

  if [[ "${public_rows}" -eq 0 ]]; then
    fail "artifacts/public-artifacts.tsv has no public artifact rows"
  fi
}

verify_no_public_core_markers_in_tree() {
  local root="$1"
  local label="$2"
  local forbidden_re='koffi|protobuf|libuv|(^|[^[:alnum:]_])caf([^[:alnum:]_]|$)|runtime-ffi|proto/|[.]proto'
  local matches

  matches="$(
    grep -RInIE \
      --exclude='*.dll' \
      --exclude='*.dylib' \
      --exclude='*.so' \
      --exclude='*.a' \
      --exclude='*.lib' \
      --exclude='*.o' \
      --exclude='*.rlib' \
      --exclude='*.rmeta' \
      --exclude='*.tgz' \
      --exclude='*.tar.gz' \
      "${forbidden_re}" \
      "${root}" \
      | head -n 20 || true
  )"
  if [[ -n "${matches}" ]]; then
    echo "[verify-public-surface] public core marker leaked in ${label}" >&2
    echo "${matches}" >&2
    exit 1
  fi
}

verify_no_public_core_markers_in_archive() {
  local artifact="$1"
  local label="$2"
  local tmp_root

  tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-artifact.XXXXXX")"
  case "${artifact}" in
    *.tar.gz|*.tgz)
      COPYFILE_DISABLE=1 tar -xzf "${artifact}" -C "${tmp_root}"
      ;;
    *)
      fail "unsupported archive for public core marker scan: ${artifact#"${repo_root}"/}"
      ;;
  esac
  verify_no_public_core_markers_in_tree "${tmp_root}" "${label}"
  rm -rf "${tmp_root}"
}

verify_bun_archive_has_no_runtime_package_dependencies() {
  local artifact="$1"
  local tmp_root package_json

  tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-bun.XXXXXX")"
  COPYFILE_DISABLE=1 tar -xzf "${artifact}" -C "${tmp_root}"
  package_json="${tmp_root}/package/package.json"
  [[ -f "${package_json}" ]] || fail "Bun package archive is missing package/package.json"

  python3 - "${package_json}" <<'PY'
import json
import sys

package_json = sys.argv[1]
with open(package_json, "r", encoding="utf-8") as fh:
    package = json.load(fh)

for key in ("dependencies", "optionalDependencies", "peerDependencies"):
    deps = package.get(key) or {}
    if deps:
        rendered = ", ".join(sorted(deps))
        raise SystemExit(f"Bun package has runtime dependency entries in {key}: {rendered}")
PY
  rm -rf "${tmp_root}"
}

verify_archive_has_no_package_dependencies() {
  local artifact="$1"
  local label="$2"
  local tmp_root package_json

  tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-package.XXXXXX")"
  COPYFILE_DISABLE=1 tar -xzf "${artifact}" -C "${tmp_root}"
  package_json="${tmp_root}/package/package.json"
  [[ -f "${package_json}" ]] || fail "${label} archive is missing package/package.json"

  python3 - "${package_json}" "${label}" <<'PY'
import json
import sys

package_json = sys.argv[1]
label = sys.argv[2]
with open(package_json, "r", encoding="utf-8") as fh:
    package = json.load(fh)

for key in ("dependencies", "optionalDependencies", "peerDependencies"):
    deps = package.get(key) or {}
    if deps:
        rendered = ", ".join(sorted(deps))
        raise SystemExit(f"{label} package has dependency entries in {key}: {rendered}")
PY
  rm -rf "${tmp_root}"
}

verify_electron_archive_has_expected_runtime_package_dependencies() {
  local artifact="$1"
  local tmp_root package_json expected_node_dependency

  tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-electron.XXXXXX")"
  COPYFILE_DISABLE=1 tar -xzf "${artifact}" -C "${tmp_root}"
  package_json="${tmp_root}/package/package.json"
  [[ -f "${package_json}" ]] || fail "Electron package archive is missing package/package.json"

  expected_node_dependency="https://raw.githubusercontent.com/phuong-tran/coakka-publish/main/$(public_manifest_path_for_label "runtime Node package")"

  python3 - "${package_json}" "${expected_node_dependency}" <<'PY'
import json
import sys

package_json = sys.argv[1]
expected_node_dependency = sys.argv[2]
with open(package_json, "r", encoding="utf-8") as fh:
    package = json.load(fh)

deps = package.get("dependencies") or {}
expected = {"coakka-v2-connector-node": expected_node_dependency}
if deps != expected:
    rendered = ", ".join(f"{name}={value}" for name, value in sorted(deps.items()))
    raise SystemExit(f"Electron package has unexpected runtime dependencies: {rendered}")

for key in ("optionalDependencies", "peerDependencies"):
    extra = package.get(key) or {}
    if extra:
        rendered = ", ".join(sorted(extra))
        raise SystemExit(f"Electron package has dependency entries in {key}: {rendered}")
PY
  rm -rf "${tmp_root}"
}

verify_logger_electron_archive_has_expected_package_dependencies() {
  local artifact="$1"
  local tmp_root package_json

  tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-logger-electron.XXXXXX")"
  COPYFILE_DISABLE=1 tar -xzf "${artifact}" -C "${tmp_root}"
  package_json="${tmp_root}/package/package.json"
  [[ -f "${package_json}" ]] || fail "logger Electron package archive is missing package/package.json"

  python3 - "${package_json}" <<'PY'
import json
import sys

package_json = sys.argv[1]
with open(package_json, "r", encoding="utf-8") as fh:
    package = json.load(fh)

deps = package.get("dependencies") or {}
expected = {
    "coakka-logger-node": "https://raw.githubusercontent.com/phuong-tran/coakka-publish/main/logger/node/releases/1.2.1+f50756ebff0d/coakka-logger-node-1.2.1.tgz"
}
if deps != expected:
    rendered = ", ".join(f"{name}={value}" for name, value in sorted(deps.items()))
    raise SystemExit(f"logger Electron package has unexpected dependencies: {rendered}")

for key in ("optionalDependencies", "peerDependencies"):
    extra = package.get(key) or {}
    if extra:
        rendered = ", ".join(sorted(extra))
        raise SystemExit(f"logger Electron package has dependency entries in {key}: {rendered}")
PY
  rm -rf "${tmp_root}"
}

verify_current_bun_tauri_public_boundary() {
  local bun_rel electron_rel logger_bun_rel logger_electron_rel logger_tauri_rel tauri_rel

  verify_no_public_core_markers_in_tree "${repo_root}/README.md" "root README"
  verify_no_public_core_markers_in_tree "${repo_root}/docs/releases/2026-07-23-logger-bun-1.2.1-6fdcc69.md" "logger Bun release note"
  verify_no_public_core_markers_in_tree "${repo_root}/docs/releases/2026-07-23-logger-tauri-electron-1.2.1-3e8a6ae.md" "logger Tauri/Electron release note"
  verify_no_public_core_markers_in_tree "${repo_root}/docs/releases/2026-07-23-runtime-bun-tauri-1.3.1-247df1b.md" "Bun/Tauri release note"
  verify_no_public_core_markers_in_tree "${repo_root}/docs/releases/2026-07-23-runtime-electron-1.3.1-4e0cab0.md" "Electron release note"

  bun_rel="$(public_manifest_path_for_label "runtime Bun package")"
  electron_rel="$(public_manifest_path_for_label "runtime Electron package")"
  logger_bun_rel="$(public_manifest_path_for_label "logger Bun package")"
  logger_electron_rel="$(public_manifest_path_for_label "logger Electron package")"
  logger_tauri_rel="$(public_manifest_path_for_label "logger Tauri source package")"
  tauri_rel="$(public_manifest_path_for_label "runtime Tauri source package")"

  verify_no_public_core_markers_in_tree "${repo_root}/$(dirname "${bun_rel}")" "runtime Bun release directory"
  verify_no_public_core_markers_in_tree "${repo_root}/$(dirname "${electron_rel}")" "runtime Electron release directory"
  verify_no_public_core_markers_in_tree "${repo_root}/$(dirname "${logger_bun_rel}")" "logger Bun release directory"
  verify_no_public_core_markers_in_tree "${repo_root}/$(dirname "${logger_electron_rel}")" "logger Electron release directory"
  verify_no_public_core_markers_in_tree "${repo_root}/$(dirname "${logger_tauri_rel}")" "logger Tauri release directory"
  verify_no_public_core_markers_in_tree "${repo_root}/$(dirname "${tauri_rel}")" "runtime Tauri release directory"
  verify_no_public_core_markers_in_archive "${repo_root}/${bun_rel}" "runtime Bun artifact"
  verify_bun_archive_has_no_runtime_package_dependencies "${repo_root}/${bun_rel}"
  verify_no_public_core_markers_in_archive "${repo_root}/${electron_rel}" "runtime Electron artifact"
  verify_electron_archive_has_expected_runtime_package_dependencies "${repo_root}/${electron_rel}"
  verify_no_public_core_markers_in_archive "${repo_root}/${logger_bun_rel}" "logger Bun artifact"
  verify_archive_has_no_package_dependencies "${repo_root}/${logger_bun_rel}" "logger Bun"
  verify_no_public_core_markers_in_archive "${repo_root}/${logger_electron_rel}" "logger Electron artifact"
  verify_logger_electron_archive_has_expected_package_dependencies "${repo_root}/${logger_electron_rel}"
  verify_no_public_core_markers_in_archive "${repo_root}/${logger_tauri_rel}" "logger Tauri artifact"
  verify_no_public_core_markers_in_archive "${repo_root}/${tauri_rel}" "runtime Tauri artifact"
}

require_file "README.md"
require_file "docs/public-artifact-contract.md"
require_file "artifacts/public-artifacts.tsv"
require_file "include/coakka/v2/client.h"
require_file "include/coakka/v2/control.h"
require_file "include/coakka/v2/runtime.h"
require_file "include/coakka/v2/transport.h"
require_file "include/coakka/v2/utils.h"
require_file "native/linux-aarch64/libcoakka_runtime_v2.so"
require_file "native/linux-x86_64/libcoakka_runtime_v2.so"
require_file "native/macos-aarch64/libcoakka_runtime_v2.dylib"
require_file "native/windows-aarch64/libcoakka_runtime_v2.dll"
require_file "native/windows-x86_64/libcoakka_runtime_v2.dll"
require_file "SHA256SUMS"

(cd "${repo_root}" && shasum -a 256 -c SHA256SUMS >/dev/null)

if ! find "${repo_root}/runtime/native/releases" -mindepth 2 -maxdepth 2 \
  -name SHA256SUMS -print -quit | grep -q .; then
  echo "[verify-public-surface] missing runtime native release checksum file" >&2
  exit 1
fi

release_roots=()
for release_root in logger runtime cli demo; do
  if [[ -d "${repo_root}/${release_root}" ]]; then
    release_roots+=("${repo_root}/${release_root}")
  fi
done

while IFS= read -r -d '' sums_file; do
  release_dir="$(dirname "${sums_file}")"
  verify_sha256_file "${release_dir#"${repo_root}"/}"
done < <(find "${release_roots[@]}" -path '*/releases/*/SHA256SUMS' -print0)

verify_maven_sidecars

verify_public_artifact_manifest

if [[ "${COAKKA_PUBLIC_VERIFY_SKIP_CURRENT_BOUNDARY:-0}" != "1" ]]; then
  verify_current_bun_tauri_public_boundary
fi

if [[ -x "${repo_root}/scripts/check-native-artifact-linkage.sh" ]]; then
  "${repo_root}/scripts/check-native-artifact-linkage.sh"
fi

if [[ "${COAKKA_PUBLIC_VERIFY_SKIP_FRAMEWORK_ADAPTERS:-0}" != "1" ]]; then
  verify_framework_adapter_dependencies
fi

if [[ "${COAKKA_PUBLIC_VERIFY_SKIP_RUNTIME_JVM_BUNDLE:-0}" != "1" &&
      -x "${repo_root}/scripts/verify-runtime-jvm-native-bundle.sh" ]]; then
  "${repo_root}/scripts/verify-runtime-jvm-native-bundle.sh"
fi

scanner_inputs=(
  "${repo_root}/README.md"
  "${repo_root}/docs"
  "${repo_root}/include"
  "${repo_root}/scripts"
  "${repo_root}/artifacts/public-artifacts.tsv"
)

if [[ -n "${COAKKA_PUBLIC_SURFACE_SCANNER:-}" ]]; then
  "${COAKKA_PUBLIC_SURFACE_SCANNER}" "${scanner_inputs[@]}"
elif [[ -x "${default_scanner}" ]]; then
  "${default_scanner}" "${scanner_inputs[@]}"
else
  echo "[verify-public-surface] no public surface scanner found; skipped content scan" >&2
fi

echo "[verify-public-surface] ok"
