#!/usr/bin/env bash
# Verify the public installed-package harness, not private runtime source.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cmake -P "${root}/coakka-http-runtime/runtime-test/verify-source.cmake"
