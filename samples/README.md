# CoAkka Sample Binaries

This tree contains prebuilt evaluation binaries for focused projects whose
reviewable source and run guides live in `coakka-samples`.

The files are committed directly to `coakka-publish` and become available when
their reviewed commit is merged to `main`. This repository does not create a
GitHub Release for them.

## Available Samples

| Sample | Version | Binary lane | Public source |
| --- | --- | --- | --- |
| [Raspberry Pi camera livestream](runtime/native/rpi-camera/README.md) | `1.1.0` | `runtime/native/rpi-camera/releases/1.1.0/` | [`coakka-samples/runtime/native/rpi-camera/`](https://github.com/phuong-tran/coakka-samples/tree/main/runtime/native/rpi-camera) |

Select one archive for the exact OS and CPU, verify it against the release
`SHA256SUMS`, then follow the source repository's run guide. Binary evidence is
platform-specific; a successful build on one platform is not evidence for
another.
