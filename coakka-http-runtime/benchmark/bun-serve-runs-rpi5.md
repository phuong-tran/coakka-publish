# Bun.serve() and CoAkka Bun on Raspberry Pi 5

[Benchmark overview](README.md) · [Evidence JSON](bun-serve-evidence-rpi5.json)

Both fixtures used Bun 1.4.2 with a two-CPU budget. Three runs per
fixture were admitted.

<!-- summary:start -->
| Fixture | Median req/s | Req/s range | p99 (ms) | Peak °C / status |
| --- | --- | --- | --- | --- |
| CoAkka Bun | 80,671.7 | 78,085.9–82,057.2 | 1.937 | 47.40 |
| Bun.serve() | 47,103.2 | 46,635.0–47,140.9 | 2.192 | 45.20 |
<!-- summary:end -->

## Individual runs

<!-- attempts:start -->
| Fixture | Attempt | Warm-up (s) | Req/s | p99 (ms) | CPU (s) | Peak °C | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| CoAkka Bun | 01-bun-coakka-1 | 5 | 80,671.7 | 1.937 | — | 47.40 | Admitted |
| Bun.serve() | 01-bun-direct-1 | 5 | 47,103.2 | 2.192 | — | 45.20 | Admitted |
| CoAkka Bun | 02-bun-coakka-1 | 5 | 78,085.9 | 1.969 | — | 46.85 | Admitted |
| Bun.serve() | 02-bun-direct-1 | 5 | 46,635.0 | 2.217 | — | 45.20 | Admitted |
| CoAkka Bun | 03-bun-coakka-1 | 5 | 82,057.2 | 1.567 | — | 47.40 | Admitted |
| Bun.serve() | 03-bun-direct-1 | 5 | 47,140.9 | 2.185 | — | 45.20 | Admitted |
<!-- attempts:end -->
