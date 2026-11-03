# JVM server comparison on Raspberry Pi 5

[Measurement data and h2load output](jvm-evidence-rpi5.json) · [Reproduction notes](evidence-rpi5.md)

[Overview](README.md) · [Earlier JVM campaign](jvm-framework-runs-rpi5.md)

All workloads use server CPUs 0–1 and load-generator CPUs 2–3. The load
uses 64 HTTP/1.1 keep-alive connections through localhost for a fixed
32-byte response. Each standard run warms for 30 seconds and measures for
10 seconds. The fourth run checks sensitivity to a 40-second warm-up.

A median requires all three standard runs to pass the 90% generator CPU
ceiling and the warm-up check to pass that ceiling and stay within 5%
of the median. p99 and sampled RSS come from the throughput-median run.
Mean and peak RSS are sampled over warm-up and measurement.

| Application | Median req/s | Range req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) | Gate |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| CoAkka JVM 1.1.2 | 84,469.6 | 83,894.6–84,811.5 | 0.831 | 243.47 | 249.44 | Accepted |
| Tomcat 10.1.50 (direct servlet) | 68,051.5 | 61,803.9–73,281.2 | 1.827 | 236.64 | 260.31 | Accepted |
| Jetty 12.1.13 | 159,186.6 | 153,993.5–160,878.5 | 0.782 | 226.86 | 243.73 | Accepted |
| Spring WebFlux 6.2.19 + Reactor Netty 1.2.18 | — | — | — | — | — | 3/3 standard runs passed headroom. Warm-up delta 7.84%. |
| Spring MVC 6.2.19 + Tomcat 10.1.50 | — | — | — | — | — | 3/3 standard runs passed headroom. Warm-up delta 25.88%. |
| Undertow 2.3.26.Final | 162,458.1 | 162,065.0–162,640.4 | 0.767 | 224.91 | 233.80 | Accepted |
| Vert.x Web 4.5.34 | — | — | — | — | — | 0/3 standard runs passed headroom. Warm-up delta 1.30%. Warm-up check was client-limited. |

## Individual runs

| Application | Round | HTTP 200 | Req/s | p99 (ms) | Mean RSS (MiB) | Sampled peak RSS (MiB) | Start/max °C | Client max CPU | Status |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| CoAkka JVM 1.1.2 | 1 | 844,696 | 84,469.6 | 0.831 | 243.47 | 249.44 | 38.60/45.20 | 47.7% | Median |
| Tomcat 10.1.50 (direct servlet) | 1 | 680,515 | 68,051.5 | 1.827 | 236.64 | 260.31 | 41.35/47.40 | 47.6% | Median |
| Jetty 12.1.13 | 1 | 1,608,785 | 160,878.5 | 0.773 | 233.39 | 245.97 | 41.35/50.70 | 85.5% | Standard run |
| Spring WebFlux 6.2.19 + Reactor Netty 1.2.18 | 1 | 513,432 | 51,343.2 | 1.516 | 340.33 | 381.47 | 42.45/47.95 | 38.3% | Standard run |
| Spring MVC 6.2.19 + Tomcat 10.1.50 | 1 | 126,451 | 12,645.1 | 129.960 | 220.47 | 236.86 | 43.00/47.40 | 17.8% | Standard run |
| Undertow 2.3.26.Final | 1 | 1,626,404 | 162,640.4 | 0.781 | 225.04 | 234.83 | 40.80/50.15 | 83.6% | Standard run |
| Vert.x Web 4.5.34 | 1 | 2,055,927 | 205,592.7 | 0.349 | 241.02 | 252.23 | 40.80/51.25 | 97.2% | Standard run, client-limited |
| Vert.x Web 4.5.34 | 2 | 2,175,085 | 217,508.5 | 0.334 | 235.91 | 246.05 | 42.45/51.80 | 99.1% | Standard run, client-limited |
| Undertow 2.3.26.Final | 2 | 1,624,581 | 162,458.1 | 0.767 | 224.91 | 233.80 | 42.45/50.70 | 82.9% | Median |
| Spring MVC 6.2.19 + Tomcat 10.1.50 | 2 | 182,090 | 18,209.0 | 92.524 | 230.95 | 246.39 | 42.45/47.95 | 21.8% | Standard run |
| Spring WebFlux 6.2.19 + Reactor Netty 1.2.18 | 2 | 576,848 | 57,684.8 | 1.494 | 289.06 | 325.12 | 42.45/47.95 | 41.9% | Standard run |
| Jetty 12.1.13 | 2 | 1,591,866 | 159,186.6 | 0.782 | 226.86 | 243.73 | 41.35/50.15 | 85.8% | Median |
| Tomcat 10.1.50 (direct servlet) | 2 | 732,812 | 73,281.2 | 1.628 | 236.83 | 264.23 | 41.90/48.50 | 49.5% | Standard run |
| CoAkka JVM 1.1.2 | 2 | 848,115 | 84,811.5 | 0.827 | 242.99 | 249.59 | 41.90/46.85 | 48.1% | Standard run |
| Spring WebFlux 6.2.19 + Reactor Netty 1.2.18 | 3 | 406,393 | 40,639.3 | 1.974 | 289.49 | 318.88 | 40.80/47.95 | 34.0% | Standard run |
| Spring MVC 6.2.19 + Tomcat 10.1.50 | 3 | 156,663 | 15,666.3 | 108.156 | 222.91 | 238.59 | 40.80/47.40 | 18.3% | Standard run |
| Undertow 2.3.26.Final | 3 | 1,620,650 | 162,065.0 | 0.898 | 224.59 | 234.06 | 42.45/50.70 | 85.6% | Standard run |
| Vert.x Web 4.5.34 | 3 | 2,109,606 | 210,960.6 | 0.343 | 235.27 | 246.62 | 41.35/51.25 | 98.1% | Standard run, client-limited |
| CoAkka JVM 1.1.2 | 3 | 838,946 | 83,894.6 | 0.851 | 243.36 | 249.72 | 41.90/46.85 | 48.1% | Standard run |
| Tomcat 10.1.50 (direct servlet) | 3 | 618,039 | 61,803.9 | 1.919 | 237.75 | 259.36 | 42.45/47.40 | 43.8% | Standard run |
| Jetty 12.1.13 | 3 | 1,539,935 | 153,993.5 | 0.811 | 230.66 | 244.48 | 41.90/50.70 | 80.6% | Standard run |
| Jetty 12.1.13 | 4 | 1,594,270 | 159,427.0 | 0.781 | 236.08 | 245.84 | 42.45/51.80 | 85.0% | 40 s warm-up check |
| Spring WebFlux 6.2.19 + Reactor Netty 1.2.18 | 4 | 473,189 | 47,318.9 | 1.692 | 345.80 | 387.56 | 41.35/48.50 | 35.8% | 40 s warm-up check |
| Spring MVC 6.2.19 + Tomcat 10.1.50 | 4 | 197,210 | 19,721.0 | 80.389 | 235.61 | 252.08 | 41.90/47.95 | 19.8% | 40 s warm-up check |
| Undertow 2.3.26.Final | 4 | 1,653,611 | 165,361.1 | 0.767 | 232.59 | 238.89 | 40.80/51.80 | 87.7% | 40 s warm-up check |
| Vert.x Web 4.5.34 | 4 | 2,137,121 | 213,712.1 | 0.338 | 242.37 | 251.69 | 42.45/51.80 | 99.1% | 40 s warm-up check, client-limited |
| CoAkka JVM 1.1.2 | 4 | 846,223 | 84,622.3 | 0.826 | 243.85 | 249.45 | 40.80/46.85 | 47.2% | 40 s warm-up check |
| Tomcat 10.1.50 (direct servlet) | 4 | 657,563 | 65,756.3 | 1.843 | 239.68 | 260.61 | 42.45/47.95 | 46.2% | 40 s warm-up check |
