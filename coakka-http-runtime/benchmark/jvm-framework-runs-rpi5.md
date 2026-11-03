# JVM framework comparison on Raspberry Pi 5

[Benchmark overview](README.md) · [Methodology](benchmark-rpi5.md)

The Pi 5 ran OpenJDK 21.0.12.1 and h2load 1.64.0. All three applications
served the same 32-byte GET /fixed response over HTTP/1.1 keep-alive on
loopback. Each timed interval lasted 10 seconds. The server ran on CPU
0 or CPUs 0–1, with the remaining CPUs assigned to the load generator.

CoAkka used the public 1.1.2 JVM package with one active I/O loop. Undertow
used one I/O thread per assigned CPU and a nonblocking handler. Vert.x
Web used one router verticle per assigned CPU and its Linux ARM64 native
transport. This fixed-response workload does not measure framework
performance under business workloads.

The median uses three 30-second warm-up rounds. A separate 40-second
warm-up check must stay within 5% of their throughput median and pass
the same load-generator gate. The 90% client ceiling applies to the
busiest assigned CPU, including sampled intervals. Latency and RSS in
a summary row come from its throughput-median round. RSS is the
time-weighted whole-process average over warm-up and load; peak is the
highest observed sample.

One initial 64-connection Vert.x attempt exceeded the harness request-log
ceiling (2,139,461 responses against a 2,000,000-row limit) and was
excluded. The retry increased only the log capacity.
A separate startup attempt was excluded before load when the native
transport image could not be verified.

## Per-run results

All timed runs below started below 50 °C, completed only HTTP 200
responses and reported no throttling. Warm-up checks and client-limited
observations are listed for audit and excluded from the median.

| Connections | CPUs | Application | Run | HTTP 200 | Req/s | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Sampled peak (MiB) | Start/max °C | Client max CPU | Note |
| ---: | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| 64 | 1 | CoAkka JVM 1.1.2 | 1 | 736,954 | 73,695.4 | 0.855 | 0.922 | 1.076 | 148.72 | 150.73 | 41.35/47.40 | 30.2% | Median |
| 64 | 1 | CoAkka JVM 1.1.2 | 2 | 741,931 | 74,193.1 | 0.846 | 0.916 | 1.133 | 148.73 | 150.88 | 41.90/46.85 | 31.1% |  |
| 64 | 1 | CoAkka JVM 1.1.2 | 3 | 735,350 | 73,535.0 | 0.856 | 0.925 | 1.082 | 148.51 | 150.78 | 43.00/46.85 | 30.6% |  |
| 64 | 1 | CoAkka JVM 1.1.2 | 4 | 738,327 | 73,832.7 | 0.852 | 0.919 | 1.134 | 148.83 | 151.11 | 42.45/45.75 | 29.8% | 40 s warm-up; Δ 0.19% |
| 64 | 1 | Undertow 2.3.26.Final | 1 | 973,926 | 97,392.6 | 0.644 | 0.696 | 0.745 | 147.17 | 153.03 | 43.55/48.50 | 53.5% |  |
| 64 | 1 | Undertow 2.3.26.Final | 2 | 951,566 | 95,156.6 | 0.661 | 0.710 | 0.751 | 147.41 | 151.12 | 41.90/47.95 | 52.0% | Median |
| 64 | 1 | Undertow 2.3.26.Final | 3 | 902,707 | 90,270.7 | 0.707 | 0.762 | 0.814 | 150.56 | 154.56 | 42.45/47.40 | 49.0% |  |
| 64 | 1 | Undertow 2.3.26.Final | 4 | 944,483 | 94,448.3 | 0.665 | 0.723 | 0.777 | 152.80 | 156.25 | 40.80/47.40 | 51.5% | 40 s warm-up; Δ 0.74% |
| 64 | 1 | Vert.x Web 4.5.34 | 1 | 1,128,534 | 112,853.4 | 0.558 | 0.600 | 0.636 | 156.42 | 159.56 | 41.35/49.05 | 72.2% | Median |
| 64 | 1 | Vert.x Web 4.5.34 | 2 | 1,142,459 | 114,245.9 | 0.552 | 0.593 | 0.628 | 157.33 | 160.88 | 42.45/47.95 | 54.1% |  |
| 64 | 1 | Vert.x Web 4.5.34 | 3 | 1,121,631 | 112,163.1 | 0.563 | 0.603 | 0.642 | 156.51 | 159.55 | 43.00/47.95 | 62.0% |  |
| 64 | 1 | Vert.x Web 4.5.34 | 4 | 1,135,082 | 113,508.2 | 0.556 | 0.594 | 0.628 | 156.82 | 159.47 | 41.90/46.85 | 61.5% | 40 s warm-up; Δ 0.58% |
| 64 | 2 | CoAkka JVM 1.1.2 | 1 | 851,546 | 85,154.6 | 0.747 | 0.777 | 0.821 | 245.96 | 249.84 | 40.80/47.95 | 48.1% |  |
| 64 | 2 | CoAkka JVM 1.1.2 | 2 | 847,331 | 84,733.1 | 0.748 | 0.780 | 0.851 | 242.67 | 249.55 | 41.90/47.40 | 46.7% | Median |
| 64 | 2 | CoAkka JVM 1.1.2 | 3 | 843,071 | 84,307.1 | 0.752 | 0.782 | 0.841 | 243.23 | 249.56 | 43.55/47.40 | 46.1% |  |
| 64 | 2 | CoAkka JVM 1.1.2 | 4 | 847,382 | 84,738.2 | 0.751 | 0.780 | 0.826 | 244.91 | 249.72 | 43.00/47.40 | 47.2% | 40 s warm-up; Δ 0.01% |
| 64 | 2 | Undertow 2.3.26.Final | 1 | 1,531,221 | 153,122.1 | 0.413 | 0.515 | 0.913 | 223.64 | 234.23 | 43.00/51.80 | 83.0% |  |
| 64 | 2 | Undertow 2.3.26.Final | 2 | 1,437,900 | 143,790.0 | 0.408 | 0.550 | 0.968 | 233.82 | 241.83 | 43.00/51.25 | 80.2% |  |
| 64 | 2 | Undertow 2.3.26.Final | 3 | 1,508,050 | 150,805.0 | 0.408 | 0.491 | 0.882 | 233.38 | 242.02 | 42.45/50.70 | 81.9% | Median |
| 64 | 2 | Undertow 2.3.26.Final | 4 | 1,535,891 | 153,589.1 | 0.393 | 0.451 | 0.817 | 235.83 | 242.22 | 41.90/52.35 | 82.8% | 40 s warm-up; Δ 1.85% |
| 64 | 2 | Vert.x Web 4.5.34 | 1 | 2,009,810 | 200,981.0 | 0.304 | 0.334 | 0.361 | 239.08 | 248.36 | 40.25/50.70 | 97.3% | Client-limited |
| 64 | 2 | Vert.x Web 4.5.34 | 2 | 2,144,147 | 214,414.7 | 0.284 | 0.312 | 0.338 | 241.14 | 250.80 | 43.55/51.80 | 98.2% | Client-limited |
| 64 | 2 | Vert.x Web 4.5.34 | 3 | 2,151,570 | 215,157.0 | 0.283 | 0.315 | 0.345 | 239.00 | 248.17 | 42.45/52.35 | 99.1% | Client-limited |
| 64 | 2 | Vert.x Web 4.5.34 | 4 | 2,176,504 | 217,650.4 | 0.279 | 0.307 | 0.335 | 242.38 | 250.45 | 42.45/52.90 | 99.1% | 40 s warm-up; Δ 1.51%, Client-limited |
