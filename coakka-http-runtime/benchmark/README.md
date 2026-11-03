# HTTP Runtime 1.1.2 on Raspberry Pi 5

The physical Raspberry Pi 5 ran the public HTTP Runtime 1.1.2 packages.
Each connector ran three times under a configuration fixed before the
measurement campaign. Each row reports the median-throughput run and
uses latency and memory from that same run. The range includes all
three accepted runs.

[Methodology](benchmark-rpi5.md) · [Results for each accepted run](benchmark-runs-rpi5.md)

| Connector | Median req/s | Req/s range | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Peak RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Python | 74,891.3 | 74,175.0–75,147.8 | 0.813 | 1.056 | 1.143 | 46.20 | 46.20 |
| Node.js | 87,583.2 | 87,235.6–88,585.0 | 0.709 | 0.894 | 0.959 | 72.62 | 72.62 |
| Bun | 81,569.7 | 80,947.9–82,715.2 | 0.752 | 0.869 | 1.496 | 70.36 | 73.38 |
| Go | 95,972.1 | 94,832.4–96,123.7 | 0.638 | 0.813 | 1.162 | 25.75 | 39.69 |
| JVM | 83,272.5 | 81,284.2–84,387.4 | 0.763 | 0.789 | 0.811 | 249.33 | 249.33 |

Each timed run used 64 clients, HTTP/1.1 keep-alive, a 32-byte response,
and a 10-second loopback load. The server used CPUs 0–1 and the load generator
used CPUs 2–3. Connectors used AUTO and one active I/O event loop. Start
temperatures were 44.10–46.85°C. The highest measured temperature during
load was 49.60°C. All accepted runs reported throttle state `0x0`.
RSS is the whole server process after load. Peak RSS is its high-water mark.

### Native C SDK 1.1.2

| Requests/s | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Peak RSS (MiB) |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 161,727.6 | 0.376 | 0.546 | 0.607 | 19.38 | 19.38 |

The native C app used the public SDK with the same workload and CPU partition.
It explicitly configured two I/O event loops and used one host event reader.
The table reports the median-throughput run of three accepted runs
(161,344.0–162,291.2 requests/s), with latency and memory
from that same run. All 4,853,628 native responses were HTTP 200.

## JVM framework comparison

The active comparison uses two server CPUs and 64 concurrent connections
on Pi 5 loopback. These are separate runs from the connector confirmation
table above. RSS is sampled over warm-up and load. One-CPU comparisons are
deferred. Earlier one-CPU runs are available in the per-run record.

| Connections | CPUs | Application | Median req/s | Req/s range | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Sampled peak (MiB) |
| ---: | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 64 | 2 | CoAkka JVM 1.1.2 | 84,733.1 | 84,307.1–85,154.6 | 0.748 | 0.780 | 0.851 | 242.67 | 249.55 |
| 64 | 2 | Undertow 2.3.26.Final | 150,805.0 | 143,790.0–153,122.1 | 0.408 | 0.491 | 0.882 | 233.38 | 242.02 |
| 64 | 2 | Vert.x Web 4.5.34: not admitted | — | — | — | — | — | — | — |

[See every comparison round, including median selections, p99 and RSS](jvm-framework-runs-rpi5.md).

## Go, Node.js, Bun and Python framework comparison

This campaign compared CoAkka 1.1.2 with eight framework fixtures.
The server had a two-CPU budget and used 64 HTTP/1.1 connections over Pi 5 localhost.
p99 and sampled RSS come from the throughput-median run.

| Host | Application | Median req/s | Req/s range | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Go | CoAkka 1.1.2 | 94,664.1 | 94,495.1–95,231.8 | 1.232 | 26.65 | 37.88 |
| Go | Chi 5.3.2 | 90,899.9 | 90,743.5–90,915.4 | 2.271 | 13.61 | 14.14 |
| Go | Gin 1.12.0 | 90,462.2 | 90,110.8–90,671.2 | 2.469 | 17.31 | 17.47 |
| Python | CoAkka 1.1.2 | 74,337.7 | 74,321.5–74,681.5 | 1.211 | 49.80 | 49.80 |
| Python | FastAPI 0.141.1 + Uvicorn 0.52.4 | 29,218.6 | 29,018.1–29,306.6 | 3.942 | 128.74 | 128.75 |
| Python | Starlette 1.6.0 + Uvicorn 0.52.4 | 48,678.7 | 48,668.4–49,168.2 | 2.230 | 99.19 | 99.20 |
| Node.js | CoAkka 1.1.2 | 87,038.7 | 86,391.2–89,017.8 | 0.974 | 73.30 | 73.44 |
| Node.js | Express 5.2.1 | 5,336.6 | 5,323.3–5,419.4 | 19.063 | 123.25 | 126.08 |
| Node.js | Fastify 5.12.4 | 16,721.1 | 16,095.2–17,101.2 | 4.197 | 85.39 | 86.95 |
| Bun | CoAkka 1.1.2 | 82,150.7 | 78,714.4–85,151.6 | 1.469 | 72.14 | 73.61 |
| Bun | Elysia 1.4.30 | 44,384.3 | 42,287.7–45,645.7 | 2.075 | 53.28 | 55.06 |
| Bun | Hono 4.13.7 | 42,018.5 | 40,215.5–43,785.4 | 2.312 | 49.02 | 50.38 |

[View all 36 individual runs](framework-runs-rpi5.md#per-run-results).

## Bun.serve() paired comparison

This campaign paired CoAkka Bun 1.1.2 with Bun.serve() using a fetch handler.
The server had a two-CPU budget and used 64 HTTP/1.1 connections over Pi 5 localhost.
p99 and sampled RSS come from the throughput-median run.

| Application | Median req/s | Range req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| CoAkka Bun 1.1.2 | 83,623.7 | 83,469.1–83,898.6 | 1.475 | 73.66 | 75.00 |
| `Bun.serve()` | 46,425.7 | 46,410.9–48,229.1 | 2.223 | 40.01 | 42.14 |

[View all 6 individual runs](bun-serve-runs-rpi5.md#individual-runs).
