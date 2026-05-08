#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
scanner="${repo_root}/scripts/scan-public-surface.sh"
tmp_root="$(mktemp -d "${TMPDIR:-/tmp}/coakka-public-surface-test.XXXXXX")"

cleanup() {
  rm -rf "${tmp_root}"
}
trap cleanup EXIT

clean_dir="${tmp_root}/clean"
mkdir -p "${clean_dir}/pkg"
printf 'public runtime package\n' >"${clean_dir}/pkg/README.txt"
(cd "${clean_dir}" && tar -czf "${tmp_root}/clean.tar.gz" pkg)
"${scanner}" "${tmp_root}/clean.tar.gz" >/dev/null

blocked_dir="${tmp_root}/blocked"
mkdir -p "${blocked_dir}/pkg"
printf '%s backend marker\n' "C""AF" >"${blocked_dir}/pkg/README.txt"
(cd "${blocked_dir}" && tar -czf "${tmp_root}/blocked.tar.gz" pkg)
if "${scanner}" "${tmp_root}/blocked.tar.gz" >/tmp/coakka-surface-test.out 2>/tmp/coakka-surface-test.err; then
  echo "[test-public-surface-scanner] expected archive payload marker to fail" >&2
  exit 1
fi
grep -q "forbidden marker" /tmp/coakka-surface-test.err

printf '%s marker\n' "lib""uv" >"${tmp_root}/not-an-archive.tar.gz"
if "${scanner}" "${tmp_root}/not-an-archive.tar.gz" >/tmp/coakka-surface-test.out 2>/tmp/coakka-surface-test.err; then
  echo "[test-public-surface-scanner] expected invalid archive blob marker to fail" >&2
  exit 1
fi
grep -q "forbidden marker" /tmp/coakka-surface-test.err

echo "[test-public-surface-scanner] ok"
