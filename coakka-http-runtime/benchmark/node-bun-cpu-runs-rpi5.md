# Node.js and Bun CPU efficiency on Raspberry Pi 5

[Benchmark overview](README.md#nodejs-and-bun-requests-per-cpu-second) · [Evidence index](evidence-rpi5.md)

Node.js v22.23.3 ran both Node.js fixtures. Bun 1.4.2 ran both Bun fixtures.
Each server ran in one process with CPUs 0–1 available. h2load v1.64.0
used CPUs 2–3 and kept 64 HTTP/1.1 connections open on localhost for
`GET /fixed`. Each successful response had HTTP status 200 and a 32-byte body.

The load generator ran continuously for 17 seconds. The measurement used a
10-second window after five seconds of warm-up. A completion timestamp is the
request start timestamp plus its duration in the h2load log. The CPU clock
measures user and system time charged to the server process across all of its
threads. The reading at the deadline was interpolated from two samples
bracketing the end of the window. The largest CPU increment across an end
bracket was under 4.8 ms. [h2load request-log format](https://github.com/nghttp2/nghttp2/blob/v1.64.0/src/h2load.cc#L947-L999)

All accepted runs started below 50 °C and reported throttle flags `0x0`.
Peak temperature and generator CPU are the highest sampled values in each
run. One setup attempt stopped before load because the Python build lacked
a process-clock wrapper. The accepted runs used the POSIX clock through
libc.

## Individual runs

Each run lists the measured request count and CPU time so readers can
recompute req/CPU-s. The overview uses the median of the three ratios
for each application.

### Node.js / CoAkka 1.1.2

#### Node CoAkka run 1

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 870,908 | 19.164820909 | 87,090.8 | 45,443.1 | 0.958 | 42.45 / 47.95 | 44.3% |

#### Node CoAkka run 2

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 871,293 | 18.977631104 | 87,129.3 | 45,911.6 | 0.951 | 41.35 / 46.30 | 41.1% |

#### Node CoAkka run 3

This is the median run.

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 874,042 | 19.080523187 | 87,404.2 | 45,808.1 | 0.960 | 40.80 / 46.30 | 48.1% |

### Node.js / Fastify 5.12.4

#### Node Fastify run 1

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 172,300 | 10.010089970 | 17,230.0 | 17,212.6 | 4.053 | 42.45 / 44.10 | 27.2% |

#### Node Fastify run 2

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 166,491 | 10.008120580 | 16,649.1 | 16,635.6 | 4.187 | 40.80 / 43.00 | 23.0% |

#### Node Fastify run 3

This is the median run.

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 166,797 | 10.008606769 | 16,679.7 | 16,665.4 | 4.184 | 40.25 / 43.55 | 23.8% |

### Bun / CoAkka 1.1.2

#### Bun CoAkka run 1

This is the median run.

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 822,720 | 18.920220474 | 82,272.0 | 43,483.6 | 1.449 | 40.25 / 46.30 | 45.9% |

#### Bun CoAkka run 2

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 831,624 | 19.173247760 | 83,162.4 | 43,374.2 | 1.458 | 40.80 / 46.30 | 48.0% |

#### Bun CoAkka run 3

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 834,877 | 19.001332085 | 83,487.7 | 43,937.8 | 1.458 | 40.25 / 46.85 | 45.4% |

### Bun / `Bun.serve()`

#### Bun Direct run 1

This is the median run.

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 482,802 | 10.035606343 | 48,280.2 | 48,108.9 | 2.151 | 41.90 / 45.20 | 35.1% |

#### Bun Direct run 2

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 509,677 | 10.035654849 | 50,967.7 | 50,786.6 | 2.074 | 41.90 / 44.10 | 57.6% |

#### Bun Direct run 3

| Requests in 10 s | Server CPU (s) | Req/s | Req/CPU-s | p99 (ms) | Start / peak (°C) | Peak generator CPU |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 468,239 | 10.031974779 | 46,823.9 | 46,674.7 | 2.211 | 40.80 / 44.10 | 35.8% |
