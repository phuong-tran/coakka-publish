# Pi 5 framework comparison for Go, Node.js, Bun and Python

[Measurement data and h2load output](framework-evidence-rpi5.json) · [Reproduction notes](evidence-rpi5.md)

[Benchmark overview](README.md) · [Methodology](benchmark-rpi5.md)

The server ran on CPU 0–1 and the load generator on CPU 2–3 on the same
Raspberry Pi 5. Each round used 64 HTTP/1.1 keep-alive connections to
`127.0.0.1`, five seconds of warm-up and ten seconds of measurement.
Every application served the same fixed 32-byte response at `GET /fixed`.
Each CoAkka lane used release 1.1.2 and one observed I/O loop. FastAPI and
Starlette used Uvicorn with two workers, uvloop and httptools. The Node.js
and Bun framework fixtures used a single application process.

The median is selected by throughput from three admitted runs. p99 and
sampled RSS come from that same run. RSS covers the whole server process
group over warm-up and load. The peak is the highest observed sample.
The 90% generator ceiling applies to the busiest sampled client CPU.
These results describe the fixed-response workload and the listed fixtures.

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

## Observed server CPU

At the throughput-median runs, CoAkka Node.js used 186.7% CPU, Express
109.8% and Fastify 102.3%. CoAkka Bun used 186.6%, Elysia 100.4% and
Hono 100.6%. Every fixture had two CPUs available. Here 100% represents
one logical CPU on average across warm-up and measurement.

## Per-run results

| Host | Application | Round | HTTP 200 | Req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) | Start/max °C | Client max CPU | Status |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| Go | CoAkka 1.1.2 | 1 | 944,951 | 94,495.1 | 1.239 | 26.68 | 37.98 | 40.80/45.75 | 51.9% | Accepted |
| Go | CoAkka 1.1.2 | 2 | 952,318 | 95,231.8 | 1.207 | 26.63 | 37.89 | 39.70/46.30 | 52.4% | Accepted |
| Go | CoAkka 1.1.2 | 3 | 946,641 | 94,664.1 | 1.232 | 26.65 | 37.88 | 39.15/45.20 | 53.7% | Median |
| Go | Chi 5.3.2 | 1 | 907,435 | 90,743.5 | 2.246 | 13.69 | 14.31 | 40.25/47.40 | 56.7% | Accepted |
| Go | Chi 5.3.2 | 2 | 908,999 | 90,899.9 | 2.271 | 13.61 | 14.14 | 41.35/47.95 | 59.8% | Median |
| Go | Chi 5.3.2 | 3 | 909,154 | 90,915.4 | 2.251 | 13.76 | 14.00 | 39.70/47.40 | 57.1% | Accepted |
| Go | Gin 1.12.0 | 1 | 901,108 | 90,110.8 | 2.418 | 17.28 | 17.73 | 40.80/47.40 | 59.6% | Accepted |
| Go | Gin 1.12.0 | 2 | 906,712 | 90,671.2 | 2.460 | 17.34 | 17.55 | 40.25/47.95 | 56.0% | Accepted |
| Go | Gin 1.12.0 | 3 | 904,622 | 90,462.2 | 2.469 | 17.31 | 17.47 | 40.25/46.85 | 56.3% | Median |
| Python | CoAkka 1.1.2 | 1 | 743,377 | 74,337.7 | 1.211 | 49.80 | 49.80 | 39.70/46.30 | 43.7% | Median |
| Python | CoAkka 1.1.2 | 2 | 743,215 | 74,321.5 | 1.250 | 49.80 | 49.80 | 41.90/47.40 | 44.9% | Accepted |
| Python | CoAkka 1.1.2 | 3 | 746,815 | 74,681.5 | 1.201 | 49.78 | 49.78 | 40.80/46.30 | 44.2% | Accepted |
| Python | FastAPI 0.141.1 + Uvicorn 0.52.4 | 1 | 292,186 | 29,218.6 | 3.942 | 128.74 | 128.75 | 40.80/47.40 | 21.1% | Median |
| Python | FastAPI 0.141.1 + Uvicorn 0.52.4 | 2 | 290,181 | 29,018.1 | 3.793 | 128.75 | 128.75 | 41.35/46.30 | 21.3% | Accepted |
| Python | FastAPI 0.141.1 + Uvicorn 0.52.4 | 3 | 293,066 | 29,306.6 | 3.906 | 128.76 | 128.77 | 42.45/45.75 | 22.2% | Accepted |
| Python | Starlette 1.6.0 + Uvicorn 0.52.4 | 1 | 486,787 | 48,678.7 | 2.230 | 99.19 | 99.20 | 40.80/46.30 | 28.3% | Median |
| Python | Starlette 1.6.0 + Uvicorn 0.52.4 | 2 | 486,684 | 48,668.4 | 2.263 | 99.22 | 99.22 | 41.35/47.40 | 29.0% | Accepted |
| Python | Starlette 1.6.0 + Uvicorn 0.52.4 | 3 | 491,682 | 49,168.2 | 2.192 | 99.20 | 99.20 | 41.35/46.30 | 29.1% | Accepted |
| Node.js | CoAkka 1.1.2 | 1 | 863,912 | 86,391.2 | 0.977 | 73.77 | 73.89 | 39.70/45.75 | 60.2% | Accepted |
| Node.js | CoAkka 1.1.2 | 2 | 870,387 | 87,038.7 | 0.974 | 73.30 | 73.44 | 39.70/46.30 | 68.9% | Median |
| Node.js | CoAkka 1.1.2 | 3 | 890,178 | 89,017.8 | 0.942 | 73.84 | 73.97 | 41.35/45.75 | 68.9% | Accepted |
| Node.js | Express 5.2.1 | 1 | 53,366 | 5,336.6 | 19.063 | 123.25 | 126.08 | 40.25/44.10 | 8.7% | Median |
| Node.js | Express 5.2.1 | 2 | 54,194 | 5,419.4 | 18.992 | 127.41 | 131.48 | 39.70/42.45 | 8.6% | Accepted |
| Node.js | Express 5.2.1 | 3 | 53,233 | 5,323.3 | 19.111 | 122.90 | 125.69 | 39.70/43.00 | 11.2% | Accepted |
| Node.js | Fastify 5.12.4 | 1 | 171,012 | 17,101.2 | 4.097 | 84.51 | 86.05 | 39.15/44.10 | 24.1% | Accepted |
| Node.js | Fastify 5.12.4 | 2 | 160,952 | 16,095.2 | 4.364 | 84.17 | 85.69 | 40.80/44.10 | 20.0% | Accepted |
| Node.js | Fastify 5.12.4 | 3 | 167,211 | 16,721.1 | 4.197 | 85.39 | 86.95 | 40.80/44.10 | 24.0% | Median |
| Bun | CoAkka 1.1.2 | 1 | 821,507 | 82,150.7 | 1.469 | 72.14 | 73.61 | 40.25/45.75 | 64.1% | Median |
| Bun | CoAkka 1.1.2 | 2 | 787,144 | 78,714.4 | 1.484 | 71.88 | 73.64 | 41.35/46.30 | 60.2% | Accepted |
| Bun | CoAkka 1.1.2 | 3 | 851,516 | 85,151.6 | 1.417 | 72.08 | 73.48 | 40.80/45.75 | 62.1% | Accepted |
| Bun | Elysia 1.4.30 | 1 | 443,843 | 44,384.3 | 2.075 | 53.28 | 55.06 | 40.80/43.55 | 50.5% | Median |
| Bun | Elysia 1.4.30 | 2 | 456,457 | 45,645.7 | 2.045 | 53.39 | 55.12 | 40.80/43.55 | 59.8% | Accepted |
| Bun | Elysia 1.4.30 | 3 | 422,877 | 42,287.7 | 2.159 | 53.31 | 55.09 | 41.35/44.65 | 54.5% | Accepted |
| Bun | Hono 4.13.7 | 1 | 437,854 | 43,785.4 | 2.277 | 48.79 | 50.33 | 39.70/44.65 | 52.6% | Accepted |
| Bun | Hono 4.13.7 | 2 | 420,185 | 42,018.5 | 2.312 | 49.02 | 50.38 | 39.70/43.55 | 50.0% | Median |
| Bun | Hono 4.13.7 | 3 | 402,155 | 40,215.5 | 2.363 | 47.70 | 49.44 | 39.15/43.00 | 52.5% | Accepted |
