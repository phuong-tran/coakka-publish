# CoAkka Raspberry Pi Camera Livestream 1.1.0

This directory is the published binary lane for the audited camera sample. It
is committed directly to `coakka-publish/main`; no GitHub Release object or
attachment is used.

## Artifacts

| Application | Platform | Archive | Verification level |
| --- | --- | --- | --- |
| Pi publisher | Raspberry Pi OS/Debian ARM64 | `coakka-camera-pi-linux-arm64.tar.gz` | live V4L2/ALSA capture and profile switching |
| Host gateway | macOS ARM64 | `coakka-camera-host-macos-arm64.tar.gz` | live control, display, video, and audio recording |
| Host gateway | Linux x86-64 | `coakka-camera-host-linux-x86_64.tar.gz` | strict native build and CLI smoke |
| Host gateway | Windows x86-64 | `coakka-camera-host-windows-x86_64.zip` | CLI, live Pi connection, and loopback UI smoke |

The Windows executable is unsigned. SmartScreen or a managed application
control policy may require an explicit local approval or organization signing.

## Verify

Download `SHA256SUMS` beside the selected archive. On macOS or Linux:

```sh
shasum -a 256 -c SHA256SUMS --ignore-missing
```

On Windows PowerShell:

```powershell
$expected = (Get-Content .\SHA256SUMS |
  Select-String 'coakka-camera-host-windows-x86_64.zip').Line.Split()[0]
$actual = (Get-FileHash .\coakka-camera-host-windows-x86_64.zip -Algorithm SHA256).Hash.ToLower()
if ($actual -ne $expected) { throw 'camera archive checksum mismatch' }
```

Read `manifest.json` for source provenance and platform evidence. Use the
[`coakka-samples` guide](https://github.com/phuong-tran/coakka-samples/tree/main/runtime-streaming-demo/rpi-camera)
for all CLI options and run commands.
