# Evidence and reproduction on Raspberry Pi 5

[Benchmark overview](README.md) · [Methodology](benchmark-rpi5.md)

| Campaign | Results | Evidence |
| --- | --- | --- |
| Release confirmation | [18 admitted runs](benchmark-runs-rpi5.md) | [JSON](release-confirmation-evidence-rpi5.json) |
| Framework comparison | [36 admitted runs](framework-runs-rpi5.md) | [JSON](framework-evidence-rpi5.json) |
| Bun.serve() | [6 admitted runs](bun-serve-runs-rpi5.md) | [JSON](bun-serve-evidence-rpi5.json) |
| CPU efficiency | [12 admitted runs](node-bun-cpu-runs-rpi5.md) | [JSON](node-bun-cpu-evidence-rpi5.json) |
| JVM | [28 scheduled runs, including thermal exclusions](jvm-full-runs-rpi5.md) | [JSON](jvm-evidence-rpi5.json) |

The JSON contains the artifact and executable digests, thermal and CPU
observations, request counts, exact latency frequencies and reasons for
excluded attempts. The report's table checks recompute the published
percentiles from that evidence.

Use the [release confirmation harness](release-confirmation/README.md)
for the six release fixtures. The [framework examples in coakka-samples](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/benchmark-rpi5)
provide starter fixtures. The comparison JSON describes this campaign's
load profile and the measured environment.

From the repository root, run the evidence and table check.

```sh
python3 -B coakka-http-runtime/benchmark/verify-evidence.py
```
