#!/usr/bin/env bash
set -euo pipefail

if [[ "$#" -ne 1 ]]; then
  printf 'usage: %s <installed-include-directory>\n' "$0" >&2
  exit 64
fi

include_dir="$1"
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
compiler="${CLANG:-clang}"

if [[ ! -f "${include_dir}/coakka/http/http.h" ]]; then
  printf 'installed CoAkka HTTP header is missing: %s\n' "${include_dir}" >&2
  exit 66
fi
command -v "${compiler}" >/dev/null 2>&1 || {
  printf 'static analyzer is unavailable: %s\n' "${compiler}" >&2
  exit 69
}

for source in native_runtime.c native_contract.c native_concurrency.c \
              test_threads_posix.c; do
  "${compiler}" --analyze -std=c11 -Wall -Wextra -Wpedantic -Wconversion \
    -Wshadow -Wsign-conversion -Wstrict-prototypes -Werror \
    -I"${include_dir}" -o /dev/null "${script_dir}/${source}"
done
