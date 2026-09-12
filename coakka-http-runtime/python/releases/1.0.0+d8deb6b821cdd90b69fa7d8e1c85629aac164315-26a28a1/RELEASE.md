# Python Release Evidence

- Source: `coakka-http-runtime-connector@26a28a1c4af3eb65334993532b4eaa082fc75438`.
- Ruff format/lint, strict mypy, public docstring and deterministic wheel
  builder checks pass.
- Exact installed wheels pass 16/16 on macOS arm64, Raspberry Pi 5 arm64 and
  Linux x86-64 UTM. Windows arm64 and x86-64 each pass 15 tests with one skip
  for a feature not advertised by the loaded Core.
- Windows selection uses the interpreter process target (`win_arm64` or
  `win-amd64`), not the ARM64 host identity reported under x64 emulation.
- Each wheel contains one Core and one integrity manifest; no private header,
  schema or compiled Python extension is present.

No PyPI coordinate, signing or portable performance claim is included.
