# HTTP Runtime 1.1.2 on Raspberry Pi 5

The physical Raspberry Pi 5 ran the public HTTP Runtime 1.1.2 packages.
Bun and JVM report the fastest accepted run from their follow-up measurements.
The other connectors report the faster of two accepted runs. Each row uses
latency and memory from the same run as throughput.

| Connector | Requests/s | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Peak RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Python | 74,996.0 | 0.820 | 1.070 | 1.175 | 46.11 | 46.11 |
| Node.js | 88,381.6 | 0.703 | 0.891 | 0.951 | 72.36 | 72.36 |
| Bun | 82,144.1 | 0.748 | 0.866 | 1.470 | 69.55 | 72.89 |
| Go | 94,552.6 | 0.647 | 0.822 | 1.173 | 25.94 | 39.17 |
| JVM | 82,848.8 | 0.769 | 0.794 | 0.810 | 245.91 | 248.05 |

Each timed run used 64 clients, HTTP/1.1 keep-alive, a 32-byte response,
and a 10-second loopback load. The server used CPUs 0–1 and the load generator
used CPUs 2–3. Connectors used AUTO and one active I/O event loop. Start
temperatures were 41.90–47.40°C. The highest measured temperature during
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

See the [benchmark methodology](benchmark-rpi5.md) for measurement and acceptance criteria.
