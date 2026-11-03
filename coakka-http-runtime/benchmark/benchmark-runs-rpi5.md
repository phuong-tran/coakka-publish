# HTTP Runtime 1.1.2 Raspberry Pi 5 runs

This table contains all three accepted runs per host used for the
[benchmark results](README.md). Each timed run lasted 10 seconds.
To verify the published result, sort the three throughput values for a host
and take the middle row. The row marked **Median** supplies the throughput,
latency, RSS and peak RSS in the published summary.

| Host | Run | HTTP 200 | Req/s | p50 (ms) | p95 (ms) | p99 (ms) | RSS (MiB) | Peak RSS (MiB) | Selected |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| Python | 1 | 741,750 | 74,175.0 | 0.840 | 1.070 | 1.202 | 46.19 | 46.19 |  |
| Python | 2 | 748,913 | 74,891.3 | 0.813 | 1.056 | 1.143 | 46.20 | 46.20 | Median |
| Python | 3 | 751,478 | 75,147.8 | 0.821 | 1.055 | 1.153 | 46.17 | 46.17 |  |
| Node.js | 1 | 875,832 | 87,583.2 | 0.709 | 0.894 | 0.959 | 72.62 | 72.62 | Median |
| Node.js | 2 | 885,850 | 88,585.0 | 0.704 | 0.889 | 0.945 | 72.83 | 72.83 |  |
| Node.js | 3 | 872,356 | 87,235.6 | 0.714 | 0.895 | 0.955 | 73.03 | 73.03 |  |
| Bun | 1 | 809,479 | 80,947.9 | 0.761 | 0.875 | 1.476 | 72.19 | 73.75 |  |
| Bun | 2 | 827,152 | 82,715.2 | 0.746 | 0.856 | 1.459 | 72.12 | 73.22 |  |
| Bun | 3 | 815,697 | 81,569.7 | 0.752 | 0.869 | 1.496 | 70.36 | 73.38 | Median |
| Go | 1 | 948,324 | 94,832.4 | 0.645 | 0.822 | 1.169 | 25.81 | 39.69 |  |
| Go | 2 | 961,237 | 96,123.7 | 0.637 | 0.812 | 1.149 | 25.70 | 38.64 |  |
| Go | 3 | 959,721 | 95,972.1 | 0.638 | 0.813 | 1.162 | 25.75 | 39.69 | Median |
| JVM | 1 | 832,725 | 83,272.5 | 0.763 | 0.789 | 0.811 | 249.33 | 249.33 | Median |
| JVM | 2 | 843,874 | 84,387.4 | 0.752 | 0.779 | 0.797 | 246.88 | 248.52 |  |
| JVM | 3 | 812,842 | 81,284.2 | 0.772 | 0.801 | 1.285 | 249.09 | 252.06 |  |
| Native C | 1 | 1,613,440 | 161,344.0 | 0.376 | 0.547 | 0.605 | 19.39 | 19.39 |  |
| Native C | 2 | 1,622,912 | 162,291.2 | 0.376 | 0.543 | 0.609 | 19.39 | 19.39 |  |
| Native C | 3 | 1,617,276 | 161,727.6 | 0.376 | 0.546 | 0.607 | 19.38 | 19.38 | Median |
