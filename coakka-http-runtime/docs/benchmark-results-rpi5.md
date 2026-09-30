# Raspberry Pi 5 HTTP Benchmark Results

No result is published yet. The physical Raspberry Pi 5 still runs its prior
Bookworm installation, and a separate boot device for a clean, fully updated
Raspberry Pi OS Lite 64-bit Trixie installation has not been provisioned. The
campaign must not run on the old installation or be replaced with a measurement
from a VM or another machine.

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
| Required OS | Raspberry Pi OS Lite 64-bit, Debian 13 Trixie; clean installation and full update pending |
| Campaign kernel and firmware | Pending capture after clean installation and reboot |
| Boot storage | Separate boot device pending; the existing NVMe installation is not a benchmark baseline |
| Source and executable digests | Pending deployment and build on the measured Pi |
| Qualification | Pending |
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
