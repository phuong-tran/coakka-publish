# Consuming The Python Wheel

Install the matching downloaded wheel with a wheel-aware installer:

```sh
python -m pip install /path/to/coakka_http-1.0.0-py3-none-manylinux_2_28_x86_64.whl
```

```python
from coakka_http import Builder, Response

service = Builder().get("/health", lambda _: Response()).start()
try:
    print(service.port)
finally:
    service.close()
```

The connector loads Core directly from the unpacked wheel installation after
checking its size, digest and process platform. `zipimport` is rejected because
an OS cannot safely load the library from inside a ZIP archive.

Use `Builder`/`Service` for buffered request/reply and the typed `Configuration`
and `Core` API for the complete capability surface. Use native leases as context
managers and keep one reader per event lane.
