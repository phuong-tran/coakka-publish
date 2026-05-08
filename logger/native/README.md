# Logger Native C/C++ Releases

This lane stores native C/C++ logger drops.

Release directories live under:

```text
logger/native/releases/<native-package-version>/
```

Each release should include:

- `coakka-logger-native-<version>.tar.gz`
- `CONSUMING.md`
- `manifest.json`
- `SHA256SUMS`

The package exposes the public C ABI plus a small C++ wrapper over that ABI.
It is not published to Maven; consumers should unpack the archive and use the
included CMake config or link directly to the platform native library.
