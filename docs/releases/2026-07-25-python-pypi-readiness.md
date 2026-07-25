# 2026-07-25 Python PyPI Runtime And Logger Release

The Python runtime and logger lanes are published on PyPI.

Current package names:

- `coakka-v2-connector`
- `coakka-logger`

Current import names:

- `coakka_v2_connector`
- `coakka_logger`

Current runtime PyPI package:

- `coakka-v2-connector==1.3.4`

Current logger PyPI package:

- `coakka-logger==1.2.2`

Runtime adoption story:

- before: public browser/API HTTP may stay real, but app-owned customer/store
  work often grows a second private backend HTTP endpoint only to get a URL,
  status mapping, retries, logs, and tests
- after: Python installs the runtime host from PyPI and routes that same work
  to a target such as `samples.customer.store`, with request/reply completion
  and deadletter visibility instead of a store REST fallback

Current GitHub Release wheel artifacts:

- `runtime/python/releases/1.3.2+caff6d6d-6d5ea58/coakka_v2_connector-1.3.2-py3-none-any.whl`
- `logger/python/releases/1.2.1+f50756ebff0d/coakka_logger-1.2.2-py3-none-any.whl`

Readiness gate from the connector release workspace:

```sh
./python/scripts/check_pypi_readiness.sh
./logger/python/scripts/check_pypi_readiness.sh
```

Runtime release verification:

- `twine check` passed for `coakka_v2_connector-1.3.2-py3-none-any.whl`
- initial PyPI upload succeeded for runtime package version `1.3.2`
- metadata patch upload succeeded for `coakka-v2-connector==1.3.3`
- onboarding link patch upload succeeded for `coakka-v2-connector==1.3.4`
- clean PyPI install smoke loaded native package `1.3.2+caff6d6d`
- PyPI `1.3.4` metadata uses the npm-style New To CoAkka onboarding and does
  not link to package-local `CONSUMING.md`, `RELEASE.md`, or
  `consumer-smoke/README.md` paths

Logger release verification:

- `twine check` passed for `coakka_logger-1.2.2-py3-none-any.whl`
- PyPI upload succeeded for `coakka-logger==1.2.2`
- clean PyPI install smoke loaded logger native package `1.2.1+f50756ebff0d`
- PyPI metadata uses the npm-style New To CoAkka onboarding, includes first-run
  install/code/output guidance, and does not link to package-local
  `CONSUMING.md`, `RELEASE.md`, or `consumer-smoke/README.md` paths
- public logger sample docs were updated from GitHub Release wheel install to
  PyPI install
