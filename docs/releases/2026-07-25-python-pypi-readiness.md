# 2026-07-25 Python PyPI Runtime Release

The Python runtime lane is published on PyPI. The logger lane remains ready for
a later PyPI publish once its separate upload is verified.

Current package names:

- `coakka-v2-connector`
- `coakka-logger`

Current import names:

- `coakka_v2_connector`
- `coakka_logger`

Current runtime PyPI package:

- `coakka-v2-connector==1.3.2`

Current GitHub Release wheel artifacts:

- `runtime/python/releases/1.3.2+caff6d6d-6d5ea58/coakka_v2_connector-1.3.2-py3-none-any.whl`
- `logger/python/releases/1.2.1+f50756ebff0d/coakka_logger-1.2.1-py3-none-any.whl`

Readiness gate from the connector release workspace:

```sh
./python/scripts/check_pypi_readiness.sh
./logger/python/scripts/check_pypi_readiness.sh
```

Runtime release verification:

- `twine check` passed for `coakka_v2_connector-1.3.2-py3-none-any.whl`
- PyPI upload succeeded for `coakka-v2-connector==1.3.2`
- clean PyPI install smoke loaded native package `1.3.2+caff6d6d`

Remaining logger release blocker before documenting normal logger PyPI install:

- logger PyPI upload and clean install smoke
- public logger sample docs updated from GitHub Release wheel install to PyPI
  install
