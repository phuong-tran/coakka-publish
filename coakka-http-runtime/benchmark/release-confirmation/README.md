<a name="reproduce-http-runtime-112-release-confirmation"></a>

# Reproduce HTTP Runtime 2.2.1 release confirmation

[Evidence index](../evidence-rpi5.md) · [Published runs](../benchmark-runs-rpi5.md)

Use a Raspberry Pi 5 with AArch64 Linux, h2load 1.64.0, Python 3.13,
Go 1.26.3, Node.js 22.0.0, Bun 1.4.2 and JDK 21. The measured h2load,
Node.js, Bun, Python and JVM executable hashes are checked by the harness.
The harness uses `taskset`, `vcgencmd` and non-interactive `sudo tee` to
set and restore the CPU governor. Run it on an idle Pi with CPUs 0–3 online,
fan state 4 and clear firmware throttle flags.

## Build the fixtures

Copy this directory to a writable work area on the Pi. Set
`BENCHMARK_GO`, `BENCHMARK_NODE`, `BENCHMARK_BUN` and `BENCHMARK_JAVA`
to exact executable paths when the tools are outside `PATH`.

```sh
bash prepare.sh
python3 -B campaign.py --check
```

`prepare.sh` downloads the released Linux ARM64 SDK and packages and checks
[their SHA-256 digests](inputs.sha256) before building or installing.
`campaign.py --check` verifies startup, the response and loaded identities
without timed load.

<a name="how-the-files-fit-together"></a>

## Run the confirmation

```sh
python3 -B campaign.py
```

The timed campaign runs Native C, Go, Node.js, Bun,
Python and JVM three times each. Run one fixture independently with
`python3 -B campaign.py --fixtures native --output results-native`.

The harness records CPU affinity, HTTP status and body, loaded files,
temperature, request latency frequencies and shutdown. Native C uses port
18431 and each connector uses port 18430 on loopback. Results are stored in
`results-confirmation/`. The campaign exits with an error when a scheduled
result is missing. All admitted runs must stay below the temperature limit
and clear the generator headroom check.
