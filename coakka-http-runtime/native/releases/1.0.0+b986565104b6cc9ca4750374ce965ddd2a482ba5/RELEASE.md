# CoAkka HTTP Runtime 1.0.0 Release Record

Status: pending

This assembly is not ready to release. Do not distribute it as a completed
five-target package or describe the benchmark as measured.

| Gate | Current evidence |
| --- | --- |
| Native source | `b986565104b6cc9ca4750374ce965ddd2a482ba5`; later runtime commits change only audit, planning, and a test fixture, not the native library source |
| macOS ARM64 | Matching-host native and installed-consumer gates passed; staged |
| Linux x86-64 | Matching-host native and installed-consumer gates passed; staged |
| Windows ARM64 | Matching-host native and installed-consumer gates passed; staged |
| Windows x86-64 | Matching-host native and installed-consumer gates passed; staged |
| Linux ARM64 | Physical Pi 5 Trixie native CTest `237/237` sequential and five complete parallel repetitions after a test-only resource-baseline correction; host component gate passed with 102 exports and both installed C/C++ consumers; staged. Trixie connector matrix remains pending |
| Raspberry Pi 5 benchmark | Pending language preparation, short qualification, cooled three-round campaign, and result review |
| Final inventory and checksums | Pending connector and benchmark gates; `manifest.json` and `SHA256SUMS` intentionally absent |

The physical Pi now boots a clean Raspberry Pi OS Lite 64-bit Trixie image from
the user-authorized SanDisk USB. Its previous NVMe Bookworm installation is
unmounted and unchanged. The Linux ARM64 installed host library is 9,350,080
bytes with SHA-256
`b71dcdf1975475c79e0738da4b0d549480fd9fc32f7721b9411940795decd1fe`.
The public [benchmark protocol](../../../docs/benchmark-rpi5.md) and
[result status](../../../docs/benchmark-results-rpi5.md) show the exact
workload and pending tables. No Pi measurement is accepted yet.

Registry package layout, upload, and production signing are outside this
assembly step. Required legal files are retained with each installed target.
