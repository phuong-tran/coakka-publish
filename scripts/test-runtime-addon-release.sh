#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tmp_parent="${COAKKA_PUBLIC_TMP_ROOT:-${repo_root}/.tmp}"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/coakka-runtime-addon.XXXXXX")"
test_output="${tmp_root}/last-command.out"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

expect_success() {
  local label="$1"
  shift
  if ! "$@" >"${test_output}" 2>&1; then
    echo "[runtime-addon-test] expected success: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

expect_failure() {
  local label="$1"
  shift
  if "$@" >"${test_output}" 2>&1; then
    echo "[runtime-addon-test] expected failure: ${label}" >&2
    cat "${test_output}" >&2
    exit 1
  fi
}

refresh_sums() {
  local release_dir="$1"
  (
    cd "${release_dir}"
    shasum -a 256 \
      coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz \
      manifest.json \
      README.md >SHA256SUMS
  )
}

update_archive_sha() {
  local release_dir="$1"
  local archive_sha
  archive_sha="$(shasum -a 256 "${release_dir}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz" | awk '{print $1}')"
  python3 - "${release_dir}/manifest.json" "${archive_sha}" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["sha256"] = sys.argv[2]
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY
  refresh_sums "${release_dir}"
}

make_fixture() {
  local name="$1"
  local fixture="${tmp_root}/${name}"
  local release_dir="${fixture}/runtime-addons/artifact-publisher-sftp/native/releases/0.1.0+abcdef0"
  local package_root="${fixture}/package/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0"
  local archive="${release_dir}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz"

  mkdir -p \
    "${release_dir}" \
    "${package_root}/include/coakka/addons" \
    "${package_root}/native/linux-x86_64" \
    "${package_root}/cmake" \
    "${package_root}/share/coakka/runtime-addons/artifact-publisher-sftp"

  printf '# Runtime addon release fixture\n' >"${release_dir}/README.md"
  printf '# Package\n' >"${package_root}/README.md"
  printf '# Consuming\n' >"${package_root}/CONSUMING.md"
  printf '# License\n' >"${package_root}/LICENSE.md"
  printf '# Third-party notices\n' >"${package_root}/THIRD_PARTY_NOTICES.md"
  printf '#pragma once\n' >"${package_root}/include/coakka/addons/artifact_publisher_sftp.h"
  printf 'fixture addon module\n' >"${package_root}/native/linux-x86_64/libcoakka_addon_artifact_publisher_sftp.so"
  printf 'fixture loader module\n' >"${package_root}/native/linux-x86_64/libcoakka_addon_artifact_publisher_sftp.so.0"
  printf 'set(CoAkkaRuntimeAddonArtifactPublisherSftp_FOUND TRUE)\n' \
    >"${package_root}/cmake/CoAkkaRuntimeAddonArtifactPublisherSftpConfig.cmake"
  cat >"${package_root}/share/coakka/runtime-addons/artifact-publisher-sftp/addon.manifest.json" <<'JSON'
{
  "manifest_version": 1,
  "addon_id": "coakka.artifact.publisher.sftp",
  "addon_kind": "bridge",
  "addon_version": "0.1.0",
  "runtime_family": "coakka-runtime-v2",
  "required_runtime_features": ["file_lane"],
  "entrypoint_symbol": "coakka_sftp_publisher_create"
}
JSON

  COPYFILE_DISABLE=1 tar -czf "${archive}" -C "${fixture}/package" \
    coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0
  local archive_sha
  archive_sha="$(shasum -a 256 "${archive}" | awk '{print $1}')"
  cat >"${release_dir}/manifest.json" <<JSON
{
  "schemaVersion": 1,
  "name": "coakka-runtime-addon-artifact-publisher-sftp-native",
  "addonId": "coakka.artifact.publisher.sftp",
  "version": "0.1.0",
  "release": "0.1.0+abcdef0",
  "sourceSnapshot": "abcdef0",
  "archive": "coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz",
  "sha256": "${archive_sha}",
  "runtimeCompatibility": {
    "abiMajor": 2,
    "minimumNativeVersion": "2.3.0",
    "requiredFeatures": ["file_lane"]
  },
  "ownedStaticDependencies": [
    { "name": "libssh2", "version": "1.11.1" },
    { "name": "crypto-provider", "version": "fixture" }
  ],
  "userInstalledNativeDependencies": false,
  "cmakeConfig": "cmake/CoAkkaRuntimeAddonArtifactPublisherSftpConfig.cmake",
  "headers": ["include/coakka/addons/artifact_publisher_sftp.h"],
  "exportedSymbols": [
    "coakka_sftp_publisher_cancel",
    "coakka_sftp_publisher_create",
    "coakka_sftp_publisher_dependency_version",
    "coakka_sftp_publisher_destroy",
    "coakka_sftp_publisher_forget",
    "coakka_sftp_publisher_get",
    "coakka_sftp_publisher_get_target",
    "coakka_sftp_publisher_start",
    "coakka_sftp_publisher_stop",
    "coakka_sftp_publisher_submit",
    "coakka_sftp_publisher_wait"
  ],
  "platforms": [
    {
      "id": "linux-x86_64",
      "library": "native/linux-x86_64/libcoakka_addon_artifact_publisher_sftp.so",
      "matchingHostRuntimeTest": "passed",
      "dynamicDependencyAudit": "passed"
    }
  ]
}
JSON
  refresh_sums "${release_dir}"
  printf '%s\n' "${release_dir}"
}

verifier="${repo_root}/scripts/verify-runtime-addon-release.py"

cmake_fixture="${tmp_root}/cmake-package"
cmake_addon_root="${cmake_fixture}/addon"
cmake_runtime_root="${cmake_fixture}/runtime"
cmake_app_root="${cmake_fixture}/app"
mkdir -p \
  "${cmake_addon_root}/cmake" \
  "${cmake_addon_root}/include" \
  "${cmake_runtime_root}" \
  "${cmake_app_root}"
cp \
  "${repo_root}/runtime-addons/artifact-publisher-sftp/native/package-template/cmake/CoAkkaRuntimeAddonArtifactPublisherSftpConfig.cmake" \
  "${cmake_addon_root}/cmake/"
case "$(uname -s)/$(uname -m)" in
  Darwin/arm64|Darwin/aarch64)
    cmake_platform="macos-aarch64"
    cmake_library="libcoakka_addon_artifact_publisher_sftp.dylib"
    ;;
  Linux/aarch64|Linux/arm64)
    cmake_platform="linux-aarch64"
    cmake_library="libcoakka_addon_artifact_publisher_sftp.so"
    ;;
  Linux/x86_64|Linux/amd64)
    cmake_platform="linux-x86_64"
    cmake_library="libcoakka_addon_artifact_publisher_sftp.so"
    ;;
  *)
    echo "[runtime-addon-test] unsupported CMake fixture host" >&2
    exit 1
    ;;
esac
mkdir -p "${cmake_addon_root}/native/${cmake_platform}"
printf 'fixture addon module\n' \
  >"${cmake_addon_root}/native/${cmake_platform}/${cmake_library}"
cat >"${cmake_runtime_root}/CoAkkaRuntimeNativeV2Config.cmake" <<'CMAKE'
if(NOT TARGET CoAkkaRuntimeNativeV2::runtime_v2)
  add_library(CoAkkaRuntimeNativeV2::runtime_v2 INTERFACE IMPORTED)
endif()
set(CoAkkaRuntimeNativeV2_FOUND TRUE)
CMAKE
cat >"${cmake_app_root}/CMakeLists.txt" <<'CMAKE'
cmake_minimum_required(VERSION 3.20)
project(CoAkkaRuntimeAddonPackageSmoke LANGUAGES C)
find_package(CoAkkaRuntimeAddonArtifactPublisherSftp CONFIG REQUIRED)
if(NOT TARGET CoAkkaRuntimeAddonArtifactPublisherSftp::artifact_publisher_sftp)
  message(FATAL_ERROR "SFTP addon imported target is missing")
endif()
CMAKE
expect_success "CMake addon package config" \
  cmake -S "${cmake_app_root}" -B "${cmake_fixture}/build" \
  -DCoAkkaRuntimeNativeV2_DIR="${cmake_runtime_root}" \
  -DCoAkkaRuntimeAddonArtifactPublisherSftp_DIR="${cmake_addon_root}/cmake"

good_release="$(make_fixture good)"
expect_success "complete runtime addon release" \
  "${verifier}" --release-dir "${good_release}" \
  --expected-addon artifact-publisher-sftp

missing_loader_release="$(make_fixture missing-loader-name)"
python3 - "${missing_loader_release}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz" <<'PY'
import pathlib
import tarfile
import sys

archive = pathlib.Path(sys.argv[1])
replacement = archive.with_suffix(".replacement")
with tarfile.open(archive, "r:gz") as source, tarfile.open(
    replacement, "w:gz"
) as target:
    for member in source.getmembers():
        if member.name.endswith("libcoakka_addon_artifact_publisher_sftp.so.0"):
            continue
        extracted = source.extractfile(member) if member.isfile() else None
        target.addfile(member, extracted)
replacement.replace(archive)
PY
update_archive_sha "${missing_loader_release}"
expect_failure "missing loader-facing SOVERSION file" \
  "${verifier}" --release-dir "${missing_loader_release}"
grep -Fq "libcoakka_addon_artifact_publisher_sftp.so.0" "${test_output}"

ambient_dependency_release="$(make_fixture ambient-dependency)"
python3 - "${ambient_dependency_release}/manifest.json" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["userInstalledNativeDependencies"] = True
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY
refresh_sums "${ambient_dependency_release}"
expect_failure "ambient native dependency" \
  "${verifier}" --release-dir "${ambient_dependency_release}"
grep -Fq "SFTP addon may not require user-installed native dependencies" \
  "${test_output}"

placeholder_dependency_release="$(make_fixture placeholder-dependency)"
python3 - "${placeholder_dependency_release}/manifest.json" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["ownedStaticDependencies"][1]["version"] = "target-package-defined"
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY
refresh_sums "${placeholder_dependency_release}"
expect_failure "placeholder static dependency version" \
  "${verifier}" --release-dir "${placeholder_dependency_release}"
grep -Fq "static dependency versions must be exact" "${test_output}"

pending_runtime_release="$(make_fixture pending-runtime)"
python3 - "${pending_runtime_release}/manifest.json" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["platforms"][0]["matchingHostRuntimeTest"] = "pending"
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY
refresh_sums "${pending_runtime_release}"
expect_failure "pending matching-host runtime test" \
  "${verifier}" --release-dir "${pending_runtime_release}"
grep -Fq "matching-host runtime test has not passed" "${test_output}"

windows_release="$(make_fixture windows-accepted)"
python3 - \
  "${windows_release}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz" \
  "${windows_release}/manifest.json" <<'PY'
import io
import json
import pathlib
import sys
import tarfile

archive = pathlib.Path(sys.argv[1])
replacement = archive.with_suffix(".replacement")
with tarfile.open(archive, "r:gz") as source, tarfile.open(
    replacement, "w:gz"
) as target:
    for member in source.getmembers():
        if member.name.endswith(".so.0"):
            continue
        member.name = member.name.replace(
            "/native/linux-x86_64", "/native/windows-x86_64"
        )
        if member.name.endswith(".so"):
            member.name = member.name.removesuffix(".so") + ".dll"
        extracted = source.extractfile(member) if member.isfile() else None
        target.addfile(member, extracted)
    root = "coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0"
    for name in (
        "libcoakka_addon_artifact_publisher_sftp.dll.a",
        "libcoakka_runtime_v2.dll.a",
    ):
        payload = b"fixture import library\n"
        info = tarfile.TarInfo(f"{root}/native/windows-x86_64/{name}")
        info.size = len(payload)
        info.mode = 0o644
        target.addfile(info, io.BytesIO(payload))
replacement.replace(archive)

path = pathlib.Path(sys.argv[2])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["platforms"][0]["id"] = "windows-x86_64"
manifest["platforms"][0]["library"] = (
    "native/windows-x86_64/libcoakka_addon_artifact_publisher_sftp.dll"
)
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY
update_archive_sha "${windows_release}"
refresh_sums "${windows_release}"
expect_success "Windows release after staging safety evidence" \
  "${verifier}" --release-dir "${windows_release}"

export_drift_release="$(make_fixture export-drift)"
python3 - "${export_drift_release}/manifest.json" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["exportedSymbols"].pop()
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY
refresh_sums "${export_drift_release}"
expect_failure "reviewed C ABI export drift" \
  "${verifier}" --release-dir "${export_drift_release}"
grep -Fq "export list differs" "${test_output}"

embedded_runtime_release="$(make_fixture embedded-runtime)"
embedded_package="${tmp_root}/embedded-package"
mkdir -p "${embedded_package}"
tar -xzf \
  "${embedded_runtime_release}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz" \
  -C "${embedded_package}"
printf 'forbidden runtime copy\n' \
  >"${embedded_package}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0/native/linux-x86_64/libcoakka_runtime_v2.so"
COPYFILE_DISABLE=1 tar -czf \
  "${embedded_runtime_release}/coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0.tar.gz" \
  -C "${embedded_package}" \
  coakka-runtime-addon-artifact-publisher-sftp-native-0.1.0
update_archive_sha "${embedded_runtime_release}"
expect_failure "embedded runtime copy" \
  "${verifier}" --release-dir "${embedded_runtime_release}"
grep -Fq "forbidden sidecar libraries" "${test_output}"

echo "[runtime-addon-test] ok"
