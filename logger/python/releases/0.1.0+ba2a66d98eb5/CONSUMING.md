# Consuming `coakka-logger`

The Python logger package is a binary wheel that embeds the native logger core.
Applications normally do not need to pass a native library path.

```python
from coakka_logger import CoakkaLoggerLevel, Logger, LoggerSpec

with Logger.start(spec=LoggerSpec(min_level=CoakkaLoggerLevel.INFO)) as logger:
    logger.info("orders", "accepted")
    record = logger.await_next(timeout_ms=1000)
```

For local development or diagnostics, an explicit native library path can be
provided:

```python
Logger.start(logger_lib_path="/abs/path/to/libcoakka_logger_core.dylib")
```

The environment override is:

```sh
COAKKA_LOGGER_LIB=/abs/path/to/libcoakka_logger_core.so
```

Current embedded native package:

- native core version: `0.1.0`
- native package version: `0.1.0+ba2a66d98eb5`
- platforms: `macos-aarch64`, `linux-aarch64`, `linux-x86_64`
