# JVM server comparison on Raspberry Pi 5

[Benchmark overview](README.md) · [Evidence JSON](jvm-evidence-rpi5.json)

Each fixture had three 30-second warm-ups and a separate 40-second check.
The four ranked fixtures passed both the measurement gates and the 5%
warm-up check. The direct Tomcat servlet was reconstructed from the
published setup. WebFlux and Spring MVC varied across repeated runs.

Jetty, Undertow and Vert.x reached the thermal cutoff. The first excluded
Jetty attempt reached 50.15°C. The resumed JVM runs used a 49.60°C cutoff,
with cooler starts for those three fixtures. Excluded attempts supply no
throughput or latency values to the ranking.

<!-- summary:start -->
| Fixture | Median req/s | Req/s range | p99 (ms) | Peak °C / status |
| --- | --- | --- | --- | --- |
| CoAkka JVM | 83,373.8 | 83,246.7–84,048.6 | 0.823 | 47.40 |
| Tomcat 10.1.50 (direct servlet) | 68,924.4 | 65,549.6–70,215.6 | 1.682 | 49.05 |
| Jetty 12.1.13 | — | — | — | Thermal cutoff |
| Spring WebFlux + Reactor Netty | 56,962.2 | 40,736.8–57,306.6 | 1.390 | 49.05 |
| Spring MVC + Tomcat | 17,250.6 | 14,905.2–18,162.8 | 87.398 | 48.50 |
| Undertow 2.3.26.Final | — | — | — | Thermal cutoff |
| Vert.x Web 4.5.34 | — | — | — | Thermal cutoff |
<!-- summary:end -->

## Individual runs

<!-- attempts:start -->
| Fixture | Attempt | Warm-up (s) | Req/s | p99 (ms) | CPU (s) | Peak °C | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| CoAkka JVM | 01-coakka-1 | 30 | 84,048.6 | 0.800 | — | 46.85 | Admitted |
| Jetty 12.1.13 | 01-jetty-1 | 30 | — | — | — | 50.15 | Thermal cutoff at 50.15°C |
| Jetty 12.1.13 | 01-jetty-2 | 30 | — | — | — | 49.60 | Interrupted while adjusting thermal controls |
| Jetty 12.1.13 | 01-jetty-3 | 30 | — | — | — | — | Interrupted while adjusting thermal controls |
| Jetty 12.1.13 | 01-jetty-4 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring MVC + Tomcat | 01-mvc-tomcat-1 | 30 | 17,250.6 | 87.398 | — | 47.95 | Admitted |
| Tomcat 10.1.50 (direct servlet) | 01-tomcat-1 | 30 | 65,549.6 | 1.774 | — | 49.05 | Admitted |
| Undertow 2.3.26.Final | 01-undertow-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Undertow 2.3.26.Final | 01-undertow-2 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Vert.x Web 4.5.34 | 01-vertx-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Vert.x Web 4.5.34 | 01-vertx-2 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring WebFlux + Reactor Netty | 01-webflux-1 | 30 | 40,736.8 | 1.993 | — | 47.95 | Admitted |
| CoAkka JVM | 02-coakka-1 | 30 | 83,246.7 | 0.811 | — | 46.85 | Admitted |
| Jetty 12.1.13 | 02-jetty-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring MVC + Tomcat | 02-mvc-tomcat-1 | 30 | 14,905.2 | 110.151 | — | 47.95 | Admitted |
| Tomcat 10.1.50 (direct servlet) | 02-tomcat-1 | 30 | 68,924.4 | 1.682 | — | 48.50 | Admitted |
| Undertow 2.3.26.Final | 02-undertow-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Vert.x Web 4.5.34 | 02-vertx-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Vert.x Web 4.5.34 | 02-vertx-2 | 30 | — | — | — | — | Interrupted while adjusting thermal controls |
| Spring WebFlux + Reactor Netty | 02-webflux-1 | 30 | 57,306.6 | 1.365 | — | 49.05 | Admitted |
| CoAkka JVM | 03-coakka-1 | 30 | 83,373.8 | 0.823 | — | 46.30 | Admitted |
| Jetty 12.1.13 | 03-jetty-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring MVC + Tomcat | 03-mvc-tomcat-1 | 30 | 18,162.8 | 88.322 | — | 48.50 | Admitted |
| Tomcat 10.1.50 (direct servlet) | 03-tomcat-1 | 30 | 70,215.6 | 1.683 | — | 48.50 | Admitted |
| Undertow 2.3.26.Final | 03-undertow-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Vert.x Web 4.5.34 | 03-vertx-1 | 30 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring WebFlux + Reactor Netty | 03-webflux-1 | 30 | 56,962.2 | 1.390 | — | 49.05 | Admitted |
| CoAkka JVM | 04-coakka-1 | 40 | 82,835.4 | 0.811 | — | 47.40 | Admitted |
| Jetty 12.1.13 | 04-jetty-1 | 40 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring MVC + Tomcat | 04-mvc-tomcat-1 | 40 | 17,006.6 | 100.152 | — | 47.95 | Admitted |
| Tomcat 10.1.50 (direct servlet) | 04-tomcat-1 | 40 | 71,182.4 | 1.686 | — | 48.50 | Admitted |
| Undertow 2.3.26.Final | 04-undertow-1 | 40 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Vert.x Web 4.5.34 | 04-vertx-1 | 40 | — | — | — | 49.60 | Thermal cutoff at 49.60°C |
| Spring WebFlux + Reactor Netty | 04-webflux-1 | 40 | 56,587.9 | 1.395 | — | 49.05 | Admitted |
<!-- attempts:end -->
