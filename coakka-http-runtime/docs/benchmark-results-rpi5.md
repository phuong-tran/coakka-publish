# Raspberry Pi 5 HTTP Benchmark Results

No result is published yet. The physical Raspberry Pi 5 now boots a clean,
fully updated Raspberry Pi OS Lite 64-bit Trixie installation from a separate
SanDisk USB. The prior Bookworm NVMe remains outside the campaign. The native
host component and all end-user sample gates pass on the board; this does not
substitute for performance qualification.

The first short benchmark qualification was rejected before completion: one
framework lane drove its dedicated load-generator CPU to 91.8% busy, above the
declared 90% ceiling. Two generator CPUs still reached 97.1% busy after the
framework sample's persistent-connection correction. None of those partial
measurements is publishable. The protocol now reserves CPU `0` for every
server and CPUs `1-3` for three load threads. The complete 19-lane short
qualification passed its validity gates, but its two-second values are
diagnostic only. A subsequent three-round campaign completed all 57 samples
with clean responses, cooldowns, governor restoration, and no throttling. It
is nevertheless rejected as release evidence: fixed-request calibration
underestimated connection startup, so some supposed ten-second measurement
intervals lasted only about two to six seconds. The runner now uses a
five-second same-connection warm-up followed by a fixed ten-second interval.
A fresh short qualification, three full randomized rounds, and performance
review remain required before any result is entered below.

The [measurement protocol](benchmark-rpi5.md) defines the fixed response,
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
| Native host component | Installed ELF SHA-256 `9b74f5b3730b207a2449cf933156881b9781d8cbcaf14734f6c47feb664384c8`; exact-source component gate passed |
| CPU placement | Server `0`; three load threads on `1-3`; busiest load CPU must stay at or below 90% busy |
| Qualification | Earlier 19-lane short validity gate passed under a superseded measurement method; revised qualification pending |
| Full campaign | Earlier 57-sample fixed-request campaign rejected for unequal measured durations; revised three-round campaign pending |

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

The final result is eligible only after a clean revised short qualification,
three matched fixed-duration rounds with same-connection warm-up, the cooldown
gate before the campaign and between every lane, exact responses, zero request
failures, no power or thermal throttling, and a complete evidence archive. A
surprising result is investigated before publication rather than averaged away.
