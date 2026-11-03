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
