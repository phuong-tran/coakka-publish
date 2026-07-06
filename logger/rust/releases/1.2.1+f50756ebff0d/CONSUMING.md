# Consuming CoAkka Rust Logger

Unpack `coakka-logger-rs-1.2.1.tar.gz` and use it as a path dependency.
The wrapper loads the bundled native logger library for the current supported
platform.

Minimal flow:

```rust
use coakka_logger_rs::{Logger, LoggerSpec};

let logger = Logger::start(LoggerSpec::new("sample-logger"))?;
logger.try_info("samples.logger.rust.basic", "{\"event\":\"hello\"}")?;
let record = logger.await_next(1000)?;
let stats = logger.stats()?;
```
