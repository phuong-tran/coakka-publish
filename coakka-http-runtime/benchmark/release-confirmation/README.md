# Reproduce HTTP Runtime 1.1.2 release confirmation

[Evidence index](../evidence-rpi5.md) · [18 accepted runs](../benchmark-runs-rpi5.md) · [Measurement data](../release-confirmation-evidence-rpi5.json)

Use a Raspberry Pi 5 with 64-bit Linux, h2load 1.64.0, Python 3.13, Node
22.0.0, Bun 1.4.2, Go 1.26.3 and JDK 21. The harness needs `taskset`,
`vcgencmd` and permission to set and restore the CPU governor through
`sudo tee`. Check that CPU 0–3 are online, the Pi is idle and the throttle
flags are clear. Run in a writable copy of this directory and allow time
for cooling between campaigns.

The runner scripts retain the measurement and identity checks. The
connector runner accepts the three runtime executable paths through
environment variables and discovers the Python package in its local
virtual environment. Rebuilding the fixtures can change executable hashes.
The evidence contains the hashes observed in the published runs. For
byte-for-byte comparison, use the [original fixture snapshot](https://github.com/phuong-tran/coakka-publish/tree/da9153e2600d0b29809c1060f05a561a5820245d/coakka-http-runtime/benchmark/release-confirmation).

## How the files fit together

- `fixture.c` serves the Native C response through the installed SDK.
  `measure-native-full.py` builds the evidence from its admitted runs.
- `server.py`, `node/server.mjs`, `bun/server.mjs`, `go-app/main.go`
  and `FixedServer.java` each serve the same 32-byte response at `/fixed`.
  At startup they report the files they loaded and the runtime settings
  they observed. The runner uses those records to verify the fixture.
- `campaign.py` selects one response mode per connector and records accepted
  attempts. It calls `run-public.py` for each attempt. That script verifies
  startup, drives warm-up and timed load, and records cleanup.
- Native C listens on `127.0.0.1:18431`. Each connector uses
  `127.0.0.1:18430`, so run the fixtures sequentially. The runners send
  SIGTERM to close the active server and restore the CPU governors.

## Build the fixtures

From the writable copy, prepare the release artifacts and compile the
Native C, Go and JVM fixtures.

```sh
set -eu
sdk_url=https://raw.githubusercontent.com/phuong-tran/coakka-publish/main/coakka-http-runtime/native/releases/1.1.2
sdk_archive=coakka-http-host-1.1.2-candidate-linux-aarch64.tar.gz
curl -fLO "$sdk_url/$sdk_archive"
printf '%s  %s\n' 1f5ea27c7f4c4e25acdc274fd49e401a5197203f9f989d96c685f38b1ceb455e "$sdk_archive" | sha256sum -c -
mkdir -p sdk classes jvm attempts
tar -xzf "$sdk_archive" -C sdk
sdk_dir="$PWD/sdk"
cc -O3 -I"$sdk_dir/include" fixture.c -L"$sdk_dir/lib" \
  -Wl,-rpath,"$sdk_dir/lib" -lcoakka_http_host -o fixture
(
  cd go-app
  CGO_ENABLED=1 CGO_CFLAGS="-I$sdk_dir/include" \
    CGO_LDFLAGS="-L$sdk_dir/lib -Wl,-rpath,$sdk_dir/lib" \
    go build -mod=readonly -o ../go-fixed .
)
python3.13 -m venv python
python/bin/pip install --only-binary=:all: coakka-http-runtime-python==1.1.2
npm install --prefix node --no-save --ignore-scripts coakka-http-runtime-node@1.1.2
npm install --prefix bun --no-save --ignore-scripts coakka-http-runtime-bun@1.1.2

maven=https://repo.maven.apache.org/maven2
jvm_release="$maven/io/github/phuong-tran/coakka/coakka-http-runtime-jvm/1.1.2"
curl -fLO "$jvm_release/coakka-http-runtime-jvm-1.1.2.jar"
curl -fLO "$jvm_release/coakka-http-runtime-jvm-1.1.2-linux-aarch64.zip"
curl -fLO "$maven/org/jetbrains/kotlin/kotlin-stdlib/2.3.10/kotlin-stdlib-2.3.10.jar"
unzip -q coakka-http-runtime-jvm-1.1.2-linux-aarch64.zip -d jvm
javac -cp coakka-http-runtime-jvm-1.1.2.jar:kotlin-stdlib-2.3.10.jar \
  -d classes FixedServer.java

export BENCHMARK_NODE="$(command -v node)"
export BENCHMARK_BUN="$(command -v bun)"
export BENCHMARK_JAVA="$(command -v java)"
```

## Run the confirmation

Run these commands sequentially.

```sh
python3 measure-native-full.py
python3 campaign.py
```

Native C writes `full-retest-runs/`. The connector driver writes
`attempts/` and `campaign.json`. Use a fresh output directory for a new
campaign. Each driver pins the server to CPUs 0–1 and h2load to CPUs 2–3.
The timed load is 10 seconds with 64 HTTP/1.1 keep-alive connections on
localhost. Native C warms for 5 seconds. Connector warm-up is 5 seconds
for Go, Node.js, Bun and Python and 30 seconds for JVM. Each driver checks
thermal admission below 50 °C before timing and checks throttle flags.
The archived release campaign used two active I/O loops for Native C and
one for each connector.

The results include the original h2load logs, thermal readings and
request latency output. To match the published table, sort the three
accepted runs by throughput and take p99, VmRSS and VmHWM from that same
median run. The public JSON preserves every latency frequency at
microsecond resolution. For p99, accumulate frequencies in ascending
order until reaching `ceil(0.99 × request_count)`.
