# HTTP Runtime 1.1.2 on Raspberry Pi 5

These results come from the qualified 1.1.2 packages on a physical Raspberry
Pi 5. For each connector, the table shows the highest throughput observed
among two accepted runs, with latency and memory from the same run. Each
configuration ran once, so these are confirmation measurements rather than
throughput medians across repeated runs.

| Connector | Requests/s | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Peak RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Python | 74,898.0 | 0.821 | 1.056 | 1.217 | 46.17 | 46.17 |
| Node.js | 88,288.3 | 0.705 | 0.902 | 0.953 | 72.83 | 72.83 |
| Bun | 82,615.4 | 0.743 | 0.865 | 1.477 | 71.64 | 72.81 |
| Go | 94,522.6 | 0.645 | 0.811 | 1.203 | 25.81 | 39.69 |
| JVM | 82,951.3 | 0.760 | 0.789 | 1.101 | 247.34 | 248.53 |

Each timed run lasted 10 seconds over loopback using HTTP/1.1 keep-alive,
64 clients and a 32-byte response. The server used AUTO on CPUs 0–1 and the
load generator used CPUs 2–3. All accepted temperature samples were below
50°C, with no throttling. RSS is the whole server process after load, and
peak RSS is its process high-water mark. The 1.1.2 pass covered connectors.

See the [benchmark methodology](benchmark-rpi5.md) for measurement and acceptance criteria.
