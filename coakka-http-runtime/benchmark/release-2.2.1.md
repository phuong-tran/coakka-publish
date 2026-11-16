# CoAkka HTTP Runtime 2.2.1 on Raspberry Pi 5

These results measure the Go, Node.js, Bun, Python and JVM 2.2.1 connectors
using Core SDK 2.2.0. The HTTP server and load generator ran on a physical
Raspberry Pi 5. The server used CPUs 0–1 and one active I/O loop. The load
generator used CPUs 2–3.

Each connector served a shared 32-byte response over 64 HTTP/1.1 keep-alive
connections on loopback. The table reports the median throughput and the
median of the per-run p99 latencies across three 10-second runs. Every run
stayed below 50 °C.

| Connector | Host runtime | Median req/s | Median p99 (ms) | Peak temperature |
| --- | --- | ---: | ---: | ---: |
| Go | Go 1.26.3 | 93,966.7 | 1.181 | 46.30 °C |
| Node.js | Node 22.22.1 | 88,164.0 | 0.945 | 47.40 °C |
| Bun | Bun 1.4.2 | 83,590.5 | 1.438 | 49.60 °C |
| Python | CPython 3.13.5 | 74,123.0 | 1.176 | 47.95 °C |
| JVM | OpenJDK 21 | 83,244.6 | 0.803 | 46.85 °C |

These measurements cover the fixed-response HTTP/1.1 workload. The benchmark
index also includes the earlier 1.1.2 framework comparisons and their
individual runs.

[Benchmark index](README.md) · [Benchmark methodology](benchmark-rpi5.md) · [Release notes](../docs/release-2.2.1.md)
