#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
publish_root="$(cd "${script_dir}/.." && pwd)"
core_root="$(cd "${publish_root}/../coakkaCoreNativeDev" && pwd)"
snapshot="$(git -C "${core_root}" rev-parse --short=8 HEAD)"
tmp_parent="${COAKKA_PUBLIC_TMP_ROOT:-${publish_root}/.tmp}"
mkdir -p "${tmp_parent}"
tmp_root="$(mktemp -d "${tmp_parent}/coakka-artifact-source-package.XXXXXX")"
trap 'rm -rf "${tmp_root}"' EXIT

output_root="${tmp_root}/publish"
cp "${publish_root}/LICENSE.md" "${output_root}.license"
mkdir -p "${output_root}"
mv "${output_root}.license" "${output_root}/LICENSE.md"
printf '# Fixture notices\n' >"${tmp_root}/THIRD_PARTY_NOTICES.md"
cat >"${tmp_root}/dependencies.json" <<'JSON'
{
  "ownedStaticDependencies": [
    { "name": "libssh2", "version": "1.11.1" },
    { "name": "fixture dependency (Windows)", "version": "1.0" }
  ]
}
JSON

platforms=(
  linux-aarch64 linux-x86_64 macos-aarch64
  windows-aarch64 windows-x86_64
)
stems=(
  sftp https s3 local_drop azure_blob gcs webdav oci_registry
  huggingface_hub github_release google_drive dropbox
)
platform_args=()
for platform in "${platforms[@]}"; do
  root="${tmp_root}/${platform}"
  mkdir -p "${root}"
  platform_args+=(--platform-root "${platform}=${root}")
  for stem in "${stems[@]}"; do
    case "${platform}" in
      linux-*)
        printf 'fixture\n' >"${root}/libcoakka_addon_artifact_publisher_${stem}.so"
        printf 'fixture\n' >"${root}/libcoakka_addon_artifact_publisher_${stem}.so.0"
        ;;
      macos-*)
        printf 'fixture\n' >"${root}/libcoakka_addon_artifact_publisher_${stem}.dylib"
        printf 'fixture\n' >"${root}/libcoakka_addon_artifact_publisher_${stem}.0.dylib"
        ;;
      windows-*)
        printf 'fixture\n' >"${root}/libcoakka_addon_artifact_publisher_${stem}.dll"
        printf 'fixture\n' >"${root}/coakka_addon_artifact_publisher_${stem}.lib"
        ;;
    esac
  done
done

"${script_dir}/package-artifact-source-release.py" \
  --core-root "${core_root}" \
  --source-snapshot "${snapshot}" \
  --third-party-notices "${tmp_root}/THIRD_PARTY_NOTICES.md" \
  --owned-dependencies "${tmp_root}/dependencies.json" \
  --output-root "${output_root}" \
  "${platform_args[@]}"

addon_count=0
while IFS= read -r slug; do
  release_dir="${output_root}/runtime-addons/${slug}/native/releases/1.1.0+${snapshot}"
  "${script_dir}/verify-runtime-addon-release.py" \
    --release-dir "${release_dir}" --expected-addon "${slug}"
  addon_count="$((addon_count + 1))"
done < <(python3 - "${core_root}/v2/addons/artifact-source-release.json" <<'PY'
import json
import pathlib
import sys

spec = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
for addon in spec["addons"]:
    print(addon["slug"])
PY
)

if [[ "${addon_count}" -ne 11 ]]; then
  echo "[artifact-source-package-test] expected 11 releases, found ${addon_count}" >&2
  exit 1
fi

local_drop_archive="${output_root}/runtime-addons/artifact-publisher-local-drop/native/releases/1.1.0+${snapshot}/coakka-runtime-addon-artifact-publisher-local-drop-native-1.1.0.tar.gz"
local_drop_manifest="${output_root}/runtime-addons/artifact-publisher-local-drop/native/releases/1.1.0+${snapshot}/manifest.json"
if tar -tzf "${local_drop_archive}" | grep -q '/native/windows-'; then
  echo "[artifact-source-package-test] Local Drop archive contains Windows binaries" >&2
  exit 1
fi
python3 - "${local_drop_manifest}" <<'PY'
import json
import pathlib
import sys

manifest = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
assert manifest["ownedStaticDependencies"] == []
assert manifest["userInstalledNativeDependencies"] is False
PY
if tar -xOzf "${local_drop_archive}" '*/CONSUMING.md' | grep -q 'load the platform libcurl'; then
  echo "[artifact-source-package-test] Local Drop documentation requires libcurl" >&2
  exit 1
fi

s3_archive="${output_root}/runtime-addons/artifact-publisher-s3/native/releases/1.1.0+${snapshot}/coakka-runtime-addon-artifact-publisher-s3-native-1.1.0.tar.gz"
if ! tar -tzf "${s3_archive}" | grep -q '/native/linux-x86_64/libcoakka_addon_artifact_publisher_https.so$'; then
  echo "[artifact-source-package-test] S3 archive is missing its private HTTPS module" >&2
  exit 1
fi

sftp_output_root="${tmp_root}/sftp-publish"
mkdir -p "${sftp_output_root}"
cp "${publish_root}/LICENSE.md" "${sftp_output_root}/LICENSE.md"
"${script_dir}/package-artifact-source-release.py" \
  --core-root "${core_root}" \
  --source-snapshot "${snapshot}" \
  --third-party-notices "${tmp_root}/THIRD_PARTY_NOTICES.md" \
  --owned-dependencies "${tmp_root}/dependencies.json" \
  --output-root "${sftp_output_root}" \
  --sftp-version 1.2.0 \
  "${platform_args[@]}"
"${script_dir}/verify-runtime-addon-release.py" \
  --release-dir "${sftp_output_root}/runtime-addons/artifact-publisher-sftp/native/releases/1.2.0+${snapshot}" \
  --expected-addon artifact-publisher-sftp

echo "[artifact-source-package-test] ok: ${addon_count} family fixtures plus SFTP"
