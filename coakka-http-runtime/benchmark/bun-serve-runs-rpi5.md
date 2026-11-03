# Bun.serve() and CoAkka Bun on Raspberry Pi 5

[Benchmark overview](README.md) · [Methodology](benchmark-rpi5.md)

Bun 1.4.2 ran on server CPU 0–1 and h2load on CPU 2–3. Both used 64
HTTP/1.1 keep-alive connections over localhost, 5 seconds of warm-up
and 10 seconds of measurement. GET /fixed returned HTTP 200 with a
32-byte binary response. The direct fixture uses Bun.serve with a
fetch handler and constructs a Response per request from the fixed payload.
CoAkka uses the public connector with the same fixture as its previous run.
Each lane ran three times with the order reversed in the second round.

RSS is the sampled time-weighted mean over warm-up and measurement.
The summary uses p99 and RSS from the throughput-median run.

| Application | Median req/s | Range req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| CoAkka Bun 1.1.2 | 83,623.7 | 83,469.1–83,898.6 | 1.475 | 73.66 | 75.00 |
| `Bun.serve()` | 46,425.7 | 46,410.9–48,229.1 | 2.223 | 40.01 | 42.14 |

## Individual runs

| Application | Round | HTTP 200 | Req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) | Start/max °C | Client max CPU | Status |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| CoAkka Bun 1.1.2 | 1 | 836,237 | 83,623.7 | 1.475 | 73.66 | 75.00 | 38.05/44.65 | 54.9% | Median |
| `Bun.serve()` | 1 | 464,109 | 46,410.9 | 2.219 | 40.50 | 42.11 | 39.70/43.55 | 51.5% | Accepted |
| `Bun.serve()` | 2 | 464,257 | 46,425.7 | 2.223 | 40.01 | 42.14 | 39.15/43.55 | 59.0% | Median |
| CoAkka Bun 1.1.2 | 2 | 838,986 | 83,898.6 | 1.618 | 72.04 | 73.44 | 40.25/45.20 | 51.0% | Accepted |
| CoAkka Bun 1.1.2 | 3 | 834,691 | 83,469.1 | 1.525 | 72.13 | 73.52 | 39.15/46.30 | 69.4% | Accepted |
| `Bun.serve()` | 3 | 482,291 | 48,229.1 | 2.156 | 39.74 | 41.88 | 40.80/44.10 | 59.6% | Accepted |
