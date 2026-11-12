<a name="http-runtime-112-raspberry-pi-5-runs"></a>

# HTTP Runtime 2.2.1 Raspberry Pi 5 runs

[Release confirmation results](README.md#release-confirmation) · [Evidence JSON](release-confirmation-evidence-rpi5.json)

The release confirmation has three admitted runs for each of six fixtures.
This table also records the initial Native C validation attempt that was
excluded because it applied a stricter terminal equality check than the
campaign protocol.

<!-- attempts:start -->
| Fixture | Attempt | Warm-up (s) | Req/s | p99 (ms) | CPU (s) | Peak °C | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Bun | 01-bun-1 | 5 | 80,858.6 | 1.569 | — | 46.30 | Admitted |
| Go | 01-go-1 | 5 | 93,728.2 | 1.212 | — | 44.10 | Admitted |
| JVM | 01-jvm-1 | 30 | 82,580.1 | 1.139 | — | 45.20 | Admitted |
| Native C | 01-native-1 | 5 | — | — | — | 46.85 | native terminal accounting or shutdown failed |
| Native C | 01-native-2 | 5 | 160,955.6 | 0.612 | — | 46.85 | Admitted |
| Node.js | 01-node-1 | 5 | 88,133.9 | 0.958 | — | 46.85 | Admitted |
| Python | 01-python-1 | 5 | 73,793.8 | 1.188 | — | 46.85 | Admitted |
| Bun | 02-bun-1 | 5 | 79,529.6 | 1.966 | — | 46.85 | Admitted |
| Go | 02-go-1 | 5 | 92,094.1 | 1.238 | — | 45.75 | Admitted |
| JVM | 02-jvm-1 | 30 | 82,954.1 | 0.980 | — | 45.75 | Admitted |
| Native C | 02-native-1 | 5 | 161,369.7 | 0.608 | — | 47.40 | Admitted |
| Node.js | 02-node-1 | 5 | 86,244.9 | 0.969 | — | 47.40 | Admitted |
| Python | 02-python-1 | 5 | 73,917.7 | 1.185 | — | 46.85 | Admitted |
| Bun | 03-bun-1 | 5 | 80,833.4 | 1.516 | — | 47.40 | Admitted |
| Go | 03-go-1 | 5 | 94,084.8 | 1.204 | — | 45.75 | Admitted |
| JVM | 03-jvm-1 | 30 | 81,772.1 | 1.093 | — | 46.30 | Admitted |
| Native C | 03-native-1 | 5 | 160,338.5 | 0.618 | — | 47.95 | Admitted |
| Node.js | 03-node-1 | 5 | 84,937.2 | 1.413 | — | 47.40 | Admitted |
| Python | 03-python-1 | 5 | 73,586.2 | 1.196 | — | 47.40 | Admitted |
<!-- attempts:end -->

## JVM framework comparison

[JVM results and all 28 scheduled runs](jvm-full-runs-rpi5.md)

## Go, Node.js, Bun and Python framework comparison

[Framework results and all 36 runs](framework-runs-rpi5.md)

## Bun.serve() paired comparison

[Bun results and all six runs](bun-serve-runs-rpi5.md)
