# Consuming CoAkka Rust Logger Spike

Unpack `coakka-logger-rs-0.1.0-spike.tar.gz` and use the package as a path
dependency. The wrapper loads the bundled native logger library for the current
supported platform.

Minimal flow:

```rust
use coakka_logger_rs::{Logger, LoggerSpec};

let logger = Logger::start(LoggerSpec::new("sample-logger"))?;
logger.try_info("samples.logger.rust.basic", "{\"event\":\"hello\"}")?;
let record = logger.await_next(1000)?;
let stats = logger.stats()?;
```
