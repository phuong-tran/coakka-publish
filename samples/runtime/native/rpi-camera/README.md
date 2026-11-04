# Raspberry Pi Camera Livestream Binaries

Version `1.1.0` packages two applications:

- `coakka_camera_pi` for Raspberry Pi OS/Debian ARM64;
- `coakka_camera_host` for macOS ARM64, Linux x86-64, and Windows x86-64.

The public source, build instructions, CLI contract, resolution controls, and
audio/no-audio recording workflow are in
[`coakka-samples/coakka-runtime/demos/rpi-camera/`](https://github.com/phuong-tran/coakka-samples/tree/main/coakka-runtime/demos/rpi-camera).

Current source has moved its HTTP/MJPEG/WebSocket and profile-control edges to
the released CoAkka HTTP Runtime native SDK `2.0.1`. The published camera
`v1.1.0` archives below predate that migration and remain byte-for-byte under
their original libuv-based evidence; this source update is not a replacement
binary release.

## Current Binary Set

[`releases/1.1.0/`](releases/1.1.0/README.md) contains the four platform
archives, manifest, checksums, release notes, and systems audit. These are
repository files on `main`, not GitHub Release attachments.

The Pi and macOS artifacts have live camera evidence. Linux x86-64 has native
build and CLI evidence. Windows x86-64 has native CLI, live Pi connection, and
loopback UI evidence; its executable is not Authenticode-signed.
