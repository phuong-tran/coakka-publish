# CoAkka Raspberry Pi Camera Livestream 1.1.2

Download the Pi publisher and macOS host for the two sides of the demo.

| Application | Verified target | Archive |
| --- | --- | --- |
| Pi publisher | Raspberry Pi 5 running Debian 13 ARM64 | [Linux ARM64](coakka-camera-pi-1.1.2-linux-arm64.tar.gz) |
| Host gateway | macOS ARM64 | [macOS ARM64](coakka-camera-host-1.1.2-macos-arm64.tar.gz) |

Save [SHA256SUMS](SHA256SUMS) beside the archive and verify it before extraction.
Run `sha256sum -c --ignore-missing SHA256SUMS` on Pi or
`shasum -a 256 -c --ignore-missing SHA256SUMS` on macOS. The selected archive
must report `OK`.

The Pi publisher requires ALSA and a system C++ library that provides
`GLIBCXX_3.4.32`. The macOS host executable requires macOS 15.2 or later.
Install FFmpeg on the host if you want to record. Follow the
[camera guide](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-runtime/demos/rpi-camera)
to configure the device, start both applications, and open the local viewer.

The binaries come from Samples commit
[`eddff0c`](https://github.com/phuong-tran/coakka-samples/commit/eddff0c596de0f431daab94515b86b01fbfff5a4).
They bundle CoAkka Runtime native `2.5.1+26f7944d` and CoAkka HTTP Runtime
native `2.0.1` (C ABI 10). The [manifest](manifest.json) records the archive
and executable checksums. The archives retain the exact bytes used for the
live test, including the qualification README.

The two archives were tested together on a physical Pi 5 and a macOS ARM64
host. The host received MJPEG frames, switched profiles over WebSocket,
recorded with and without AAC audio, and shut down cleanly with the Pi.
See the [systems audit](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-runtime/demos/rpi-camera/SYSTEMS-AUDIT.md#camera-112-archive-verification)
for the test scope. Other platform archives remain in
[v1.1.0](../1.1.0/README.md).
