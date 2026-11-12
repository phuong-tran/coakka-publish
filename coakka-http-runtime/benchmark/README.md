<a name="http-runtime-112-on-raspberry-pi-5"></a>

# HTTP Runtime 2.2.1 on Raspberry Pi 5

The November 12, 2026 measurements use the published HTTP Runtime 2.2.1
connectors and native SDK 2.2.0 with C ABI 10. Each fixture served a 32-byte
HTTP/1.1 response over 64 loopback connections. The server used CPUs 0–1
and the load generator used CPUs 2–3. The highest temperature in an
admitted run was 49.05°C.

[Methodology](benchmark-rpi5.md) · [Evidence and reproduction](evidence-rpi5.md) · [Previous benchmark in Git](https://github.com/phuong-tran/coakka-publish/tree/550a4196ac23237ae6b6375170632a15dca56eca/coakka-http-runtime/benchmark)

## Release confirmation

The results report the median of three admitted runs per fixture.
Native C used two I/O loops and one host event reader. Each connector used
one active I/O loop. p99 comes from the same run as median throughput.

<a name="native-c-sdk-112"></a>

<!-- summary:start -->
| Fixture | Median req/s | Req/s range | p99 (ms) | Peak °C / status |
| --- | --- | --- | --- | --- |
| Native C | 160,955.6 | 160,338.5–161,369.7 | 0.612 | 47.95 |
| Go | 93,728.2 | 92,094.1–94,084.8 | 1.212 | 45.75 |
| Node.js | 86,244.9 | 84,937.2–88,133.9 | 0.969 | 47.40 |
| Bun | 80,833.4 | 79,529.6–80,858.6 | 1.516 | 47.40 |
| Python | 73,793.8 | 73,586.2–73,917.7 | 1.188 | 47.40 |
| JVM | 82,580.1 | 81,772.1–82,954.1 | 1.139 | 46.30 |
<!-- summary:end -->

[All release confirmation runs](benchmark-runs-rpi5.md)

## Go, Node.js, Bun and Python framework comparison

The comparison uses the same request and CPU budget across 12 fixtures.
Each fixture has three admitted runs.

[Framework results and per-run evidence](framework-runs-rpi5.md)

## Bun.serve() paired comparison

Both fixtures used Bun 1.4.2 and ran three times under the same load profile.

[Paired results](bun-serve-runs-rpi5.md)

## Node.js and Bun requests per CPU-second

This comparison measures completed requests and process CPU time over
the same 10-second window.

[CPU efficiency results](node-bun-cpu-runs-rpi5.md)

## JVM framework comparison

Jetty, Undertow and Vert.x reached the thermal cutoff and are unranked.
The remaining four fixtures have three admitted measurements and passed
the 40-second warm-up check.

[JVM results and exclusions](jvm-full-runs-rpi5.md)
