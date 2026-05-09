# Consuming The C# Logger Connector

Install `CoAkka.Logger` from this release directory as a local NuGet source.
The package provides a small `CoAkka.Logger` API and copies the supported native
logger library into the consumer output directory.

Minimal flow:

```csharp
using CoAkka.Logger;

using var logger = LoggerHost.Start(new LoggerStartSpec("sample-logger"));
logger.TryInfo("samples.logger.csharp.basic", "{\"event\":\"hello\"}", out _);
var record = logger.AwaitNext(1000);
var stats = logger.Stats();
```
