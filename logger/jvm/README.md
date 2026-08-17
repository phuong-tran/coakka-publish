# logger/jvm

The current JVM logger is published on Maven Central as:

```text
io.github.phuong-tran.coakka:logger:1.2.2
```

Use [the runnable JVM logger samples](https://github.com/phuong-tran/coakka-samples/tree/main/logger/jvm)
for Kotlin and Java basic/pressure flows. Release identity and verification
evidence are recorded in
[`docs/releases/2026-08-17-logger-jvm-maven-central-1.2.2.md`](../../docs/releases/2026-08-17-logger-jvm-maven-central-1.2.2.md).

Published logger JVM drops live under `logger/jvm/releases/<release-dir>/`.

Current release-directory bias:

- default to the bundled native package version, for example
  `1.2.1+f50756ebff0d`
- keep the JVM artifact version and native core version inside `manifest.json`
  instead of encoding every version axis into the directory name

Each release directory should contain:

- packaged jar
- sources jar
- `README.md`
- `CONSUMING.md`
- `manifest.json`
- `SHA256SUMS`
