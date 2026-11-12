# Node.js and Bun CPU efficiency on Raspberry Pi 5

[Benchmark overview](README.md) · [Evidence JSON](node-bun-cpu-evidence-rpi5.json)

The generator ran continuously for 17 seconds. The harness counted completed
requests and process CPU time over the same 10-second window after warm-up.
The table uses the median by requests per CPU-second. Node.js used version
22.23.3 and Bun used version 1.4.2.

<!-- summary:start -->
| Fixture | Median req/s | Req/CPU-s | p99 (ms) | Peak °C / status |
| --- | --- | --- | --- | --- |
| CoAkka Node.js | 85,535.3 | 44,977.3 | 0.994 | 47.95 |
| Fastify 5.12.4 | 16,762.3 | 16,749.5 | 4.180 | 44.65 |
| CoAkka Bun | 80,560.2 | 43,266.3 | 1.926 | 47.40 |
| Bun.serve() | 46,861.7 | 46,713.1 | 2.200 | 45.20 |
<!-- summary:end -->

## Individual runs

<!-- attempts:start -->
| Fixture | Attempt | Warm-up (s) | Req/s | p99 (ms) | CPU (s) | Peak °C | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| <a name="bun-coakka-run-1"></a>CoAkka Bun | 01-bun-coakka-1 | 5 | 80,560.2 | 1.926 | 18.61962 | 47.40 | Admitted |
| <a name="bun-direct-run-1"></a>Bun.serve() | 01-bun-direct-1 | 5 | 49,197.8 | 2.162 | 10.03719 | 45.20 | Admitted |
| <a name="node-coakka-run-1"></a>CoAkka Node.js | 01-node-coakka-1 | 5 | 85,535.3 | 0.994 | 19.01743 | 46.85 | Admitted |
| <a name="node-fastify-run-1"></a>Fastify 5.12.4 | 01-node-fastify-1 | 5 | 16,710.4 | 4.165 | 10.00874 | 44.65 | Admitted |
| <a name="bun-coakka-run-2"></a>CoAkka Bun | 02-bun-coakka-1 | 5 | 80,942.4 | 1.589 | 18.67890 | 47.40 | Admitted |
| <a name="bun-direct-run-2"></a>Bun.serve() | 02-bun-direct-1 | 5 | 46,861.7 | 2.200 | 10.03182 | 45.20 | Admitted |
| <a name="node-coakka-run-2"></a>CoAkka Node.js | 02-node-coakka-1 | 5 | 86,336.1 | 0.981 | 19.01702 | 47.95 | Admitted |
| <a name="node-fastify-run-2"></a>Fastify 5.12.4 | 02-node-fastify-1 | 5 | 17,016.2 | 4.130 | 10.01013 | 44.10 | Admitted |
| <a name="bun-coakka-run-3"></a>CoAkka Bun | 03-bun-coakka-1 | 5 | 82,181.6 | 1.521 | 19.15385 | 47.40 | Admitted |
| <a name="bun-direct-run-3"></a>Bun.serve() | 03-bun-direct-1 | 5 | 46,536.1 | 2.209 | 10.03773 | 45.20 | Admitted |
| <a name="node-coakka-run-3"></a>CoAkka Node.js | 03-node-coakka-1 | 5 | 85,988.6 | 0.968 | 19.15314 | 47.95 | Admitted |
| <a name="node-fastify-run-3"></a>Fastify 5.12.4 | 03-node-fastify-1 | 5 | 16,762.3 | 4.180 | 10.00763 | 44.65 | Admitted |
<!-- attempts:end -->
