#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/../.." && pwd)"
source "${script_dir}/native_candidate.sh"

coakka_die() { printf '%s\n' "$*" >&2; exit 1; }
coakka_note() { printf '%s\n' "$*" >&2; }
coakka_require_command() {
  command -v "$1" >/dev/null 2>&1 || coakka_die "$2"
}

evidence_release="1.3.4+dc6ec284"
evidence_version="1.3.4"
coakka_evidence_tmp_dir=""

coakka_cleanup_evidence_tmp_dir() {
  if [[ -n "${coakka_evidence_tmp_dir}" && -d "${coakka_evidence_tmp_dir}" ]]; then
    rm -rf -- "${coakka_evidence_tmp_dir}"
  fi
}

trap coakka_cleanup_evidence_tmp_dir EXIT

coakka_evidence_mode() {
  case "${1:-smoke}" in
    file-lane) printf '%s\n' "file-lane-simple" ;;
    stream-lane) printf '%s\n' "stream-lane-simple" ;;
    smoke|pressure|stress|soak|connection-strategies|file-lane-simple|file-lane-owner-aware|stream-lane-simple|stream-lane-owner-aware|race|hot-reload|native-startup) printf '%s\n' "$1" ;;
    -h|--help) printf '%s\n' "help" ;;
    *) printf '%s\n' "smoke" ;;
  esac
}

coakka_print_evidence_help() {
  cat <<'EOF'
{
  "schema": "coakka.runtime.native.evidence.help.v1",
  "usage": "bash coakka-runtime/coakka-runtime-test/run.sh [smoke|pressure|stress|soak|connection-strategies|file-lane-simple|file-lane-owner-aware|stream-lane-simple|stream-lane-owner-aware|race|hot-reload|native-startup]",
  "payloadPresets": ["64K", "128K", "256K", "512K", "1M", "2M", "3M"],
  "pressurePayloadLimit": "16K",
  "requestLimitMax": 500000,
  "concurrencyUsage": "bash coakka-runtime/coakka-runtime-test/run.sh [race|hot-reload] [--threads 4] [--requests 128] [--generations 16] [--lifecycle-iterations 8] [--queue-capacity 1024] [--timeout 30s]",
  "nativeStartupUsage": "COAKKA_NATIVE_EVIDENCE_RUNTIME_PREFIX=/exact/installed/candidate COAKKA_NATIVE_EVIDENCE_BUILD_DIR=/external/owned/build bash coakka-runtime/coakka-runtime-test/run.sh native-startup",
  "nativeStartupScope": "source candidate only; no published package or prebuilt fallback",
  "installedCandidateScope": "An explicit runtime prefix/build selects the current engine for every mode; no historical package fallback."
}
EOF
}

coakka_native_platform() {
  local system machine
  system="$(uname -s)"
  machine="$(uname -m)"
  case "${system}:${machine}" in
    Darwin:arm64|Darwin:aarch64) printf '%s\n' "macos-aarch64" ;;
    Linux:aarch64|Linux:arm64) printf '%s\n' "linux-aarch64" ;;
    Linux:x86_64|Linux:amd64) printf '%s\n' "linux-x86_64" ;;
    *) coakka_die "Unsupported native evidence platform: ${system}/${machine}" ;;
  esac
}

run_from_source() {
  local tmp_dir build_dir mode executable
  tmp_dir="$(mktemp -d "${TMPDIR:-/tmp}/coakka-native-evidence.XXXXXX")"
  coakka_evidence_tmp_dir="${tmp_dir}"
  mode="$(coakka_evidence_mode "${1:-}")"

  if [[ ( "${mode}" == file-lane-* || "${mode}" == stream-lane-* ) &&
        -n "${COAKKA_NATIVE_EVIDENCE_RUNTIME_SOURCE_DIR:-}" ]]; then
    local require_option target
    if [[ "${mode}" == stream-lane-* ]]; then
      require_option="COAKKA_NATIVE_EVIDENCE_REQUIRE_STREAM_LANE"
      target="coakka_runtime_v2_stream_lane_evidence"
    else
      require_option="COAKKA_NATIVE_EVIDENCE_REQUIRE_FILE_LANE"
      target="coakka_runtime_v2_file_lane_evidence"
    fi
    build_dir="${tmp_dir}/build"
    cmake \
      -S "${script_dir}" \
      -B "${build_dir}" \
      -DCOAKKA_NATIVE_EVIDENCE_RUNTIME_SOURCE_DIR="${COAKKA_NATIVE_EVIDENCE_RUNTIME_SOURCE_DIR}" \
      -D"${require_option}"=ON \
      >/dev/null
    cmake --build "${build_dir}" --config Release \
      --target "${target}" >/dev/null
    executable="${build_dir}/${target}"
    coakka_note "starting native runtime evidence mode=${mode} path=core-source"
    if [[ "${mode}" == *-owner-aware ]]; then
      "${executable}" --owner-aware
    else
      "${executable}"
    fi
    return
  fi

  coakka_die "Select an installed SDK and build directory to run this mode. See the coakka-runtime-test README."
}

run_from_prebuilt() {
  local tmp_dir platform artifact_name release_dir package_root native_path
  coakka_require_command tar "Install tar, then retry."

  tmp_dir="$(mktemp -d "${TMPDIR:-/tmp}/coakka-native-evidence.XXXXXX")"
  coakka_evidence_tmp_dir="${tmp_dir}"

  platform="$(coakka_native_platform)"
  local requested_mode
  requested_mode="$(coakka_evidence_mode "${1:-}")"
  if [[ "${requested_mode}" == "connection-strategies" ||
        "${requested_mode}" == file-lane-* ||
        "${requested_mode}" == stream-lane-* ||
        "${requested_mode}" == "race" ||
        "${requested_mode}" == "hot-reload" ]]; then
    coakka_die "Connection-strategy, File/Stream Lane, and concurrency evidence require the current source-build harness."
  fi
  coakka_note "preparing published native runtime evidence runner platform=${platform}"
  artifact_name="coakka-runtime-native-evidence-v2-${evidence_version}-${platform}.tar.gz"
  release_dir="${repo_root}/runtime/evidence/native/releases/${evidence_release}"
  (
    cd "${release_dir}"
    checksum="$(awk -v archive="${artifact_name}" '$2 == archive { print }' SHA256SUMS)"
    [[ -n "${checksum}" ]] || coakka_die "No published checksum for ${artifact_name}"
    if command -v sha256sum >/dev/null 2>&1; then
      printf '%s\n' "${checksum}" | sha256sum -c - >&2
    else
      printf '%s\n' "${checksum}" | shasum -a 256 --check >&2
    fi
  )
  mkdir -p "${tmp_dir}/evidence"
  tar -C "${tmp_dir}/evidence" -xzf "${release_dir}/${artifact_name}"

  package_root="${tmp_dir}/evidence/coakka-runtime-native-evidence-v2-${evidence_version}-${platform}"
  native_path="${package_root}/native/${platform}"
  coakka_note "starting native runtime evidence mode=$(coakka_evidence_mode "${1:-}") path=prebuilt platform=${platform}"
  case "$(uname -s)" in
    Darwin)
      COAKKA_EVIDENCE_EXECUTION_PATH=prebuilt \
        DYLD_LIBRARY_PATH="${native_path}" \
        "${package_root}/bin/coakka-runtime-native-evidence" "$@"
      ;;
    Linux)
      COAKKA_EVIDENCE_EXECUTION_PATH=prebuilt \
        LD_LIBRARY_PATH="${native_path}" \
        "${package_root}/bin/coakka-runtime-native-evidence" "$@"
      ;;
  esac
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  coakka_print_evidence_help
elif [[ "${1:-}" == native-startup ||
        -n "${COAKKA_NATIVE_EVIDENCE_RUNTIME_PREFIX:-}" ||
        -n "${COAKKA_NATIVE_EVIDENCE_BUILD_DIR:-}" ]]; then
  run_installed_native_candidate "${script_dir}" "${repo_root}" "$@"
elif [[ "${COAKKA_NATIVE_EVIDENCE_USE_PREBUILT:-0}" == "1" ]]; then
  run_from_prebuilt "$@"
elif [[ -n "${COAKKA_NATIVE_EVIDENCE_RUNTIME_SOURCE_DIR:-}" ]]; then
  run_from_source "$@"
else
  coakka_die "Select an installed SDK and build directory. Set COAKKA_NATIVE_EVIDENCE_USE_PREBUILT=1 to run the historical published package."
fi
