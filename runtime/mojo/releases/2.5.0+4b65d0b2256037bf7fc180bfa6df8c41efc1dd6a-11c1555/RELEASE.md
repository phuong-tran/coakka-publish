# Building The Mojo Source Distribution

Build and verify the reproducible source archive with:

```sh
bash scripts/check-platform-source.sh
bash scripts/package-release.sh
bash scripts/check-package-readiness.sh
```

The archive contains the digest-locked macOS ARM64, Linux ARM64, and Windows
x86-64 native libraries. Source compilation and native payload verification
are reported separately from end-to-end connector execution on each platform.
