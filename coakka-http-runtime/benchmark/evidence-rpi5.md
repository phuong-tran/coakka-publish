# Evidence and reproduction on Raspberry Pi 5

[Benchmark overview](README.md) · [Methodology](benchmark-rpi5.md)

| Campaign | Per-run tables | Measurement data |
| --- | --- | --- |
| Release confirmation | [18 Native C and connector runs](benchmark-runs-rpi5.md) | [JSON](release-confirmation-evidence-rpi5.json) · [Reproduction](release-confirmation/README.md) |
| Go, Node.js, Bun and Python | [36 runs](framework-runs-rpi5.md#per-run-results) | [JSON](framework-evidence-rpi5.json) |
| Bun.serve() pair | [6 runs](bun-serve-runs-rpi5.md#individual-runs) | [JSON](bun-serve-evidence-rpi5.json) |
| JVM | [28 runs](jvm-full-runs-rpi5.md#individual-runs) | [JSON](jvm-evidence-rpi5.json) |

The release-confirmation export includes h2load warm-up and timed output,
thermal readings, loaded file digests, and exact latency counts in microseconds.
These counts reproduce the published percentiles without per-request timestamps.
RSS is the process VmRSS after load and VmHWM. This campaign did not capture
sampled generator CPU or a time series for RSS.

The comparison JSON exports contain the recorded workload, h2load output and sampled
RSS, temperature and generator CPU. Server argument templates use placeholders
for campaign and harness directories. The exports omit local paths, boot
identifiers and startup logs. The JVM export includes the two warm-up
checks that missed the stability gate and the Vert.x runs that exceeded
the generator CPU ceiling.

The public harness contains fixture sources, setup instructions and the
two-CPU lane configuration. Its commands reproduce the public harness
profile. The recorded campaigns used their own fixture and artifact
selection, and the JSON records the settings used for each run.

- [Harness and setup](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-http-runtime/benchmark-rpi5)
- [Two-CPU lane configuration](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/benchmark-rpi5/config/lanes-2cpu.json)
- [Runner](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/benchmark-rpi5/scripts/run-rpi5.py)

After preparing the harness on the Pi, run this from its directory.

```sh
python3 scripts/run-rpi5.py --config config/lanes-2cpu.json \
  --lane go-chi --lane go-gin --output evidence/go-frameworks
```

Choose matching lane IDs from the configuration for other public harness
runs. The recorded Bun.serve() pair and direct Tomcat fixture are separate
campaigns with their own per-run tables.
