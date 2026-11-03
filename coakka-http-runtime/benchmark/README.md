# HTTP Runtime 1.1.2 on Raspberry Pi 5

The Pi 5 server used CPU 0–1 and the load generator used CPU 2–3.
All campaigns below used 64 HTTP/1.1 keep-alive connections over localhost
and a fixed 32-byte response. For each admitted configuration, p99 and RSS
come from the throughput-median run.

| Campaign | Results | Run records |
| --- | --- | --- |
| Release confirmation | [Native C and connector baselines](#release-confirmation) | [Individual runs](benchmark-runs-rpi5.md) |
| Go, Node.js, Bun and Python | [Framework comparison](#go-nodejs-bun-and-python-framework-comparison) | [36 individual runs](framework-runs-rpi5.md#per-run-results) |
| Bun built-in server | [Bun.serve() paired comparison](#bunserve-paired-comparison) | [6 individual runs](bun-serve-runs-rpi5.md#individual-runs) |
| JVM | [JVM framework comparison](#jvm-framework-comparison) | [28 individual runs](jvm-full-runs-rpi5.md#individual-runs) |

[Methodology](benchmark-rpi5.md) · [Evidence and reproduction](evidence-rpi5.md)

## Release confirmation

Native C used two I/O event loops and one host event reader. The connector
baseline used AUTO with one active I/O event loop. These profiles differ
in execution topology. RSS in this section is measured after load, and
peak RSS is the process high-water mark.

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

### Native C SDK 1.1.2

| Requests/s | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Peak RSS (MiB) |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 161,727.6 | 0.376 | 0.546 | 0.607 | 19.38 | 19.38 |

The native C app used the public SDK with the same workload and CPU partition.
It explicitly configured two I/O event loops and used one host event reader.
The table reports the median-throughput run of three accepted runs
(161,344.0–162,291.2 requests/s), with latency and memory
from that same run. All 4,853,628 native responses were HTTP 200.

## Go, Node.js, Bun and Python framework comparison

This campaign compared CoAkka 1.1.2 with eight framework fixtures.
The server had a two-CPU budget and used 64 HTTP/1.1 connections over Pi 5 localhost.
RSS is sampled across warm-up and measurement.

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

Each server had two logical CPUs available. In the throughput-median
runs, CoAkka Node.js and Bun each averaged about 187% server CPU in a
single application process. Express, Fastify, Elysia and Hono averaged
about 100–110%. The CPU average spans warm-up and measurement. Throughput
covers the timed interval. [CPU accounting](benchmark-rpi5.md#metrics-and-accounting)

[View all 36 individual runs](framework-runs-rpi5.md#per-run-results).

## Bun.serve() paired comparison

CoAkka Bun reuses one prebuilt response. The Bun.serve() fetch fixture
creates a new Response for each request from the same fixed payload.
The server had a two-CPU budget and used 64 HTTP/1.1 connections over Pi 5 localhost.
RSS is sampled across warm-up and measurement.

| Application | Median req/s | Range req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| CoAkka Bun 1.1.2 | 83,623.7 | 83,469.1–83,898.6 | 1.475 | 73.66 | 75.00 |
| `Bun.serve()` | 46,425.7 | 46,410.9–48,229.1 | 2.223 | 40.01 | 42.14 |

The median CoAkka Bun run averaged 188.5% server CPU. The median
Bun.serve() fetch run averaged 100.3%.

[View all 6 individual runs](bun-serve-runs-rpi5.md#individual-runs).

## JVM framework comparison

This campaign includes Tomcat with a direct servlet, Spring MVC on Tomcat,
Jetty, Undertow, WebFlux, Vert.x and CoAkka JVM. The three measurement
rounds used a 30-second warm-up. A separate 40-second warm-up check had
to stay within 5% of the median and pass the load-generator gate.
RSS is sampled across warm-up and measurement.

| Application | Median req/s | Range req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) | Gate |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| CoAkka JVM 1.1.2 | 84,469.6 | 83,894.6–84,811.5 | 0.831 | 243.47 | 249.44 | Accepted |
| Tomcat 10.1.50 (direct servlet) | 68,051.5 | 61,803.9–73,281.2 | 1.827 | 236.64 | 260.31 | Accepted |
| Jetty 12.1.13 | 159,186.6 | 153,993.5–160,878.5 | 0.782 | 226.86 | 243.73 | Accepted |
| Spring WebFlux 6.2.19 + Reactor Netty 1.2.18 | — | — | — | — | — | 3/3 standard runs passed headroom. Warm-up delta 7.84%. |
| Spring MVC 6.2.19 + Tomcat 10.1.50 | — | — | — | — | — | 3/3 standard runs passed headroom. Warm-up delta 25.88%. |
| Undertow 2.3.26.Final | 162,458.1 | 162,065.0–162,640.4 | 0.767 | 224.91 | 233.80 | Accepted |
| Vert.x Web 4.5.34 | — | — | — | — | — | 0/3 standard runs passed headroom. Warm-up delta 1.30%. Warm-up check was client-limited. |

[View all 28 individual runs](jvm-full-runs-rpi5.md#individual-runs) · [Earlier JVM campaign](jvm-framework-runs-rpi5.md)
