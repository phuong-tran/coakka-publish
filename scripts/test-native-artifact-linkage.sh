#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
checker="${repo_root}/scripts/check-native-artifact-linkage.sh"
source_release="${repo_root}/runtime-addons/artifact-publisher-azure-blob/native/releases/1.1.0+d1032f6d"
tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-native-linkage-test.XXXXXX")"
trap 'rm -rf "${tmp_root}"' EXIT

make_fixture() {
  local name="$1"
  local fixture="${tmp_root}/${name}"
  local release="${fixture}/runtime-addons/artifact-publisher-azure-blob/native/releases/1.1.0+d1032f6d"

  mkdir -p "${fixture}/scripts" "${fixture}/artifacts" "${release}"
  cp "${checker}" "${fixture}/scripts/check-native-artifact-linkage.sh"
  cp "${source_release}/manifest.json" "${release}/manifest.json"
  cp "${source_release}/coakka-runtime-addon-artifact-publisher-azure-blob-native-1.1.0.tar.gz" "${release}/"
  printf 'public\truntime addon linkage fixture\t%s\tfixture\n' \
    'runtime-addons/artifact-publisher-azure-blob/native/releases/1.1.0+d1032f6d/coakka-runtime-addon-artifact-publisher-azure-blob-native-1.1.0.tar.gz' \
    >"${fixture}/artifacts/public-artifacts.tsv"
  printf '%s\n' "${fixture}"
}

good_fixture="$(make_fixture declared-sibling)"
COAKKA_NATIVE_LINKAGE_TMP_ROOT="${good_fixture}/.tmp/linkage" \
  "${good_fixture}/scripts/check-native-artifact-linkage.sh" >/dev/null

parallel_fixture="$(make_fixture parallel-default-roots)"
"${parallel_fixture}/scripts/check-native-artifact-linkage.sh" >/dev/null &
first_pid=$!
"${parallel_fixture}/scripts/check-native-artifact-linkage.sh" >/dev/null &
second_pid=$!
wait "${first_pid}"
wait "${second_pid}"

bad_fixture="$(make_fixture undeclared-sibling)"
python3 - "${bad_fixture}/runtime-addons/artifact-publisher-azure-blob/native/releases/1.1.0+d1032f6d/manifest.json" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
for platform in manifest["platforms"]:
    platform.pop("bundledLibraries", None)
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY

if COAKKA_NATIVE_LINKAGE_TMP_ROOT="${bad_fixture}/.tmp/linkage" \
    "${bad_fixture}/scripts/check-native-artifact-linkage.sh" >/dev/null 2>&1; then
  echo "[native-linkage-test] undeclared sibling dependency was accepted" >&2
  exit 1
fi

curl_fixture="$(make_fixture undeclared-ambient-curl)"
python3 - "${curl_fixture}/runtime-addons/artifact-publisher-azure-blob/native/releases/1.1.0+d1032f6d/manifest.json" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
manifest = json.loads(path.read_text(encoding="utf-8"))
manifest["userInstalledNativeDependencies"] = False
path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
PY

if COAKKA_NATIVE_LINKAGE_TMP_ROOT="${curl_fixture}/.tmp/linkage" \
    "${curl_fixture}/scripts/check-native-artifact-linkage.sh" >/dev/null 2>&1; then
  echo "[native-linkage-test] undeclared ambient libcurl was accepted" >&2
  exit 1
fi

echo "[native-linkage-test] ok"
