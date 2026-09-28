# Consuming The Python Wheel

```sh
python -m pip install /path/to/coakka_http-1.0.0-py3-none-manylinux_2_28_x86_64.whl
```

Choose the wheel matching the target. Do not use `zipimport`: native libraries
must be unpacked by a wheel-aware installer. No separate native package is
required.

