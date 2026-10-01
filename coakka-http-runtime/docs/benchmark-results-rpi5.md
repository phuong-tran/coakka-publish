# Raspberry Pi 5 HTTP Benchmark Results

No result is published yet. The physical Raspberry Pi 5 now boots a clean,
fully updated Raspberry Pi OS Lite 64-bit Trixie installation from a separate
SanDisk USB. The prior Bookworm NVMe remains outside the campaign. The native
host component and all end-user sample gates pass on the board; this does not
substitute for performance qualification.

The first short benchmark qualification was rejected before completion: one
framework lane drove its dedicated load-generator CPU to 91.8% busy, above the
declared 90% ceiling. None of its partial measurements is a publishable result.
The protocol now reserves two CPUs for the load generator and two for the
server, applies that partition identically to every lane, and checks the
busiest generator CPU. A complete short qualification and all three full
rounds must be rerun before any result is entered below.

The [measurement protocol](benchmark-rpi5.md) defines the fixed request,
host-inlined CoAkka lanes, framework comparisons, CPU placement, cooldown,
qualification, and evidence requirements. Each ecosystem is compared only with
frameworks in that ecosystem; the tables do not rank languages against one
another. Language-standard HTTP servers are not comparison lanes.

## Machine And Evidence Status

| Field | Status |
| --- | --- |
| Board | Raspberry Pi 5 Model B Rev 1.1 |
| CPU | Four ARM Cortex-A76 cores, up to 2.4 GHz |
| Memory | 16 GiB |
| Architecture | Linux AArch64 |
| Required OS | Raspberry Pi OS Lite 64-bit, Debian 13 Trixie; clean installation and full update complete |
| Kernel and firmware | `6.18.50+rpt-rpi-2712`; no throttle reported during preparation or rejected qualification; final campaign recaptures both |
| Boot storage | SanDisk USB 250 GB (`/dev/sda2` root); prior SK hynix NVMe unmounted |
| Native host component | Installed ELF SHA-256 `b71dcdf1975475c79e0738da4b0d549480fd9fc32f7721b9411940795decd1fe`; benchmark source digest refreshed before the next qualification |
| CPU placement | Server `0-1`; two load threads on `2-3`; busiest load CPU must stay at or below 90% busy |
| Qualification | Prior single-generator-CPU attempt rejected; revised complete qualification pending |
| Full campaign | Pending |

## Per-Ecosystem Results

Every entry below is pending. In particular, no throughput, latency, CPU, or
memory value is implied by the presence of a lane. After the campaign passes
all gates, replace these status tables with the generated per-ecosystem tables
from the checked-in summarizer, preserving its machine and evidence record.

| Ecosystem | CoAkka application surface | Framework comparisons | Status |
| --- | --- | --- | --- |
| C | Host-inlined | GNU libmicrohttpd | Pending |
| C++ | Host-inlined | cpp-httplib | Pending |
| Go | Host-inlined | Chi, Gin | Pending |
| Kotlin/JVM | Host-inlined | Netty, Jetty | Pending |
| Python | Host-inlined | FastAPI and Starlette on Uvicorn | Pending |
| Node.js | Host-inlined | Express, Fastify | Pending |
| Bun | Host-inlined | Elysia, Hono | Pending |

The final result is eligible only after a clean short qualification, three
matched full rounds, the cooldown gate after calibration and between every
lane, exact responses, zero request failures, no power or thermal throttling,
and a complete evidence archive. A surprising result is investigated before
publication rather than averaged away.
