# Pi 5 framework comparison for Go, Node.js, Bun and Python

[Benchmark overview](README.md) · [Evidence JSON](framework-evidence-rpi5.json)

All 36 runs were admitted. The fixtures used 64 persistent HTTP/1.1
connections, five seconds of warm-up and ten seconds of measurement.
FastAPI 0.141.1 and Starlette 1.6.0 used Uvicorn 0.52.4 with two workers,
uvloop 0.22.1 and httptools 0.8.0. The Python fixture was confirmed in a
follow-up block with the same workload and admission rules.

<!-- summary:start -->
| Fixture | Median req/s | Req/s range | p99 (ms) | Peak °C / status |
| --- | --- | --- | --- | --- |
| CoAkka Go | 94,542.1 | 93,643.1–94,691.2 | 1.167 | 46.85 |
| Chi 5.3.2 | 90,781.9 | 90,219.5–91,305.8 | 2.221 | 48.50 |
| Gin 1.12.0 | 90,823.4 | 90,743.4–91,062.7 | 2.421 | 47.95 |
| CoAkka Node.js | 86,189.5 | 85,535.9–86,673.9 | 0.976 | 47.40 |
| Express 5.2.1 | 5,379.2 | 5,367.1–5,420.3 | 18.907 | 44.65 |
| Fastify 5.12.4 | 16,822.7 | 16,722.9–17,185.8 | 4.232 | 44.65 |
| CoAkka Bun | 81,444.2 | 79,965.6–81,449.4 | 1.777 | 47.40 |
| Elysia 1.4.30 | 43,499.0 | 40,911.9–45,610.7 | 2.136 | 45.20 |
| Hono 4.13.7 | 44,166.0 | 40,222.0–49,360.5 | 2.239 | 45.20 |
| CoAkka Python | 74,056.4 | 73,962.4–74,072.3 | 1.171 | 47.40 |
| FastAPI + Uvicorn | 28,801.3 | 28,634.0–28,878.6 | 3.776 | 46.85 |
| Starlette + Uvicorn | 48,492.6 | 47,857.4–48,547.7 | 2.162 | 47.40 |
<!-- summary:end -->

## Observed server CPU

Per-process CPU and sampled RSS are recorded in the evidence. Two logical
CPUs were available to each server. In the CPU fields, 100% represents one
logical CPU.

## Per-run results

<!-- attempts:start -->
| Fixture | Attempt | Warm-up (s) | Req/s | p99 (ms) | CPU (s) | Peak °C | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| CoAkka Bun | 01-bun-coakka-1 | 5 | 79,965.6 | 1.962 | — | 47.40 | Admitted |
| Elysia 1.4.30 | 01-bun-elysia-1 | 5 | 45,610.7 | 2.053 | — | 45.20 | Admitted |
| Hono 4.13.7 | 01-bun-hono-1 | 5 | 44,166.0 | 2.239 | — | 45.20 | Admitted |
| Chi 5.3.2 | 01-go-chi-1 | 5 | 90,219.5 | 2.216 | — | 47.40 | Admitted |
| CoAkka Go | 01-go-coakka-1 | 5 | 94,542.1 | 1.167 | — | 46.85 | Admitted |
| Gin 1.12.0 | 01-go-gin-1 | 5 | 90,743.4 | 2.417 | — | 47.95 | Admitted |
| CoAkka Node.js | 01-node-coakka-1 | 5 | 86,189.5 | 0.976 | — | 47.40 | Admitted |
| Express 5.2.1 | 01-node-express-1 | 5 | 5,420.3 | 18.958 | — | 44.65 | Admitted |
| Fastify 5.12.4 | 01-node-fastify-1 | 5 | 17,185.8 | 4.097 | — | 44.65 | Admitted |
| CoAkka Python | 01-python-coakka-1 | 5 | 73,962.4 | 1.181 | — | 47.40 | Admitted |
| FastAPI + Uvicorn | 01-python-fastapi-1 | 5 | 28,801.3 | 3.776 | — | 46.30 | Admitted |
| Starlette + Uvicorn | 01-python-starlette-1 | 5 | 47,857.4 | 2.245 | — | 46.85 | Admitted |
| CoAkka Bun | 02-bun-coakka-1 | 5 | 81,444.2 | 1.777 | — | 47.40 | Admitted |
| Elysia 1.4.30 | 02-bun-elysia-1 | 5 | 43,499.0 | 2.136 | — | 44.10 | Admitted |
| Hono 4.13.7 | 02-bun-hono-1 | 5 | 49,360.5 | 2.050 | — | 45.20 | Admitted |
| Chi 5.3.2 | 02-go-chi-1 | 5 | 90,781.9 | 2.221 | — | 47.95 | Admitted |
| CoAkka Go | 02-go-coakka-1 | 5 | 94,691.2 | 1.176 | — | 46.85 | Admitted |
| Gin 1.12.0 | 02-go-gin-1 | 5 | 91,062.7 | 2.415 | — | 47.95 | Admitted |
| CoAkka Node.js | 02-node-coakka-1 | 5 | 85,535.9 | 0.992 | — | 47.40 | Admitted |
| Express 5.2.1 | 02-node-express-1 | 5 | 5,367.1 | 18.982 | — | 44.10 | Admitted |
| Fastify 5.12.4 | 02-node-fastify-1 | 5 | 16,822.7 | 4.232 | — | 44.65 | Admitted |
| CoAkka Python | 02-python-coakka-1 | 5 | 74,056.4 | 1.171 | — | 46.85 | Admitted |
| FastAPI + Uvicorn | 02-python-fastapi-1 | 5 | 28,634.0 | 3.787 | — | 46.85 | Admitted |
| Starlette + Uvicorn | 02-python-starlette-1 | 5 | 48,547.7 | 2.137 | — | 47.40 | Admitted |
| CoAkka Bun | 03-bun-coakka-1 | 5 | 81,449.4 | 1.724 | — | 46.85 | Admitted |
| Elysia 1.4.30 | 03-bun-elysia-1 | 5 | 40,911.9 | 2.237 | — | 44.65 | Admitted |
| Hono 4.13.7 | 03-bun-hono-1 | 5 | 40,222.0 | 2.382 | — | 44.10 | Admitted |
| Chi 5.3.2 | 03-go-chi-1 | 5 | 91,305.8 | 2.187 | — | 48.50 | Admitted |
| CoAkka Go | 03-go-coakka-1 | 5 | 93,643.1 | 1.209 | — | 46.85 | Admitted |
| Gin 1.12.0 | 03-go-gin-1 | 5 | 90,823.4 | 2.421 | — | 47.95 | Admitted |
| CoAkka Node.js | 03-node-coakka-1 | 5 | 86,673.9 | 0.971 | — | 47.40 | Admitted |
| Express 5.2.1 | 03-node-express-1 | 5 | 5,379.2 | 18.907 | — | 44.10 | Admitted |
| Fastify 5.12.4 | 03-node-fastify-1 | 5 | 16,722.9 | 4.226 | — | 44.65 | Admitted |
| CoAkka Python | 03-python-coakka-1 | 5 | 74,072.3 | 1.174 | — | 46.30 | Admitted |
| FastAPI + Uvicorn | 03-python-fastapi-1 | 5 | 28,878.6 | 3.756 | — | 46.30 | Admitted |
| Starlette + Uvicorn | 03-python-starlette-1 | 5 | 48,492.6 | 2.162 | — | 47.40 | Admitted |
<!-- attempts:end -->
