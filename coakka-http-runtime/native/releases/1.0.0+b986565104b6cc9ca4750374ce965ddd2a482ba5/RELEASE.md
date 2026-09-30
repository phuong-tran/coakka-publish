# CoAkka HTTP Runtime 1.0.0 Release Record

Status: pending

This assembly is not ready to release. Do not distribute it as a completed
five-target package or describe the benchmark as measured.

| Gate | Current evidence |
| --- | --- |
| Native source | `b986565104b6cc9ca4750374ce965ddd2a482ba5`; later runtime commits change only audit and planning documents |
| macOS ARM64 | Matching-host native and installed-consumer gates passed; staged |
| Linux x86-64 | Matching-host native and installed-consumer gates passed; staged |
| Windows ARM64 | Matching-host native and installed-consumer gates passed; staged |
| Windows x86-64 | Matching-host native and installed-consumer gates passed; staged |
| Linux ARM64 | Pending clean Raspberry Pi OS Trixie installation, rebuild, and matching-host gates |
| Raspberry Pi 5 benchmark | Pending clean OS, qualification, cooled three-round campaign, and result review |
| Final inventory and checksums | Pending five-target assembly and verifier pass |

The current Raspberry Pi installation is Bookworm on its only NVMe boot
device. The clean Trixie campaign requires separate boot media; no result
from the old installation or a virtual machine is a substitute. The public
[benchmark protocol](../../../docs/benchmark-rpi5.md) and
[result status](../../../docs/benchmark-results-rpi5.md) show the exact
workload and pending tables.

Registry package layout, upload, and production signing are outside this
assembly step. Required legal files are retained with each installed target.
