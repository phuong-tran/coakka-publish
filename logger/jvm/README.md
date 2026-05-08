# logger/jvm

Published logger JVM drops live under `logger/jvm/releases/<release-dir>/`.

Current release-directory bias:

- default to the bundled native package version, for example
  `0.1.0+7ae87f3c4ee7`
- keep the JVM artifact version and native core version inside `manifest.json`
  instead of encoding every version axis into the directory name

Each release directory should contain:

- packaged jar
- sources jar
- `README.md`
- `CONSUMING.md`
- `manifest.json`
- `SHA256SUMS`
