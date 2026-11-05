# Raspberry Pi Camera Livestream Binaries

[Version 1.1.2](releases/1.1.2/README.md) pairs the Raspberry Pi 5 publisher
with the macOS ARM64 host. Both applications use CoAkka HTTP Runtime native
SDK 2.0.1 for the camera HTTP and WebSocket endpoints.

| Application | Verified target | Archive |
| --- | --- | --- |
| Pi publisher | Raspberry Pi 5, Debian 13 ARM64 | [Download](releases/1.1.2/coakka-camera-pi-1.1.2-linux-arm64.tar.gz) |
| Host gateway | macOS ARM64 | [Download](releases/1.1.2/coakka-camera-host-1.1.2-macos-arm64.tar.gz) |

Check each download against [SHA256SUMS](releases/1.1.2/SHA256SUMS).
See the [camera sample guide](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-runtime/demos/rpi-camera)
for device setup, a shared token, and the run commands.

The [v1.1.0 archives](releases/1.1.0/README.md) remain available for their
original target and verification scope, including Linux x86-64 and Windows
x86-64 hosts.
