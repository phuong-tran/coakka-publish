# 2026-07-25 Python PyPI Readiness

The Python runtime and logger lanes are ready for a later PyPI publish once a
PyPI account and upload token are available.

Current package names:

- `coakka-v2-connector`
- `coakka-logger`

Current import names:

- `coakka_v2_connector`
- `coakka_logger`

Current GitHub Release wheel artifacts:

- `runtime/python/releases/1.3.1+0da8c2d9-8ff6f32/coakka_v2_connector-1.3.1-py3-none-any.whl`
- `logger/python/releases/1.2.1+f50756ebff0d/coakka_logger-1.2.1-py3-none-any.whl`

Readiness gate from the connector release workspace:

```sh
./python/scripts/check_pypi_readiness.sh
./logger/python/scripts/check_pypi_readiness.sh
```

Release blocker before documenting normal PyPI install:

- PyPI/TestPyPI account and upload token
- TestPyPI upload and clean install smoke
- real PyPI upload and clean install smoke
- public sample docs updated from GitHub Release wheel install to PyPI install
