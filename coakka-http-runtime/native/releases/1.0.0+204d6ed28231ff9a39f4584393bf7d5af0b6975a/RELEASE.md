# Native Release Evidence

- Source: `coakka-http-runtime@204d6ed28231ff9a39f4584393bf7d5af0b6975a`.
- ABI 8, 176 exact public exports, one installed public header.
- Installed consumer: 8/8 on every target; Windows and Pi repetitions are
  recorded at the source owner.
- Linux: Rocky 8 producer, GLIBC 2.28 ceiling, optional runtime liburing.
- Windows: static CRT, approved system imports, CFG/NX/ASLR/security cookie.
- macOS: arm64, minimum deployment target 11.0; actual macOS 11 execution was
  unavailable.
- Windows x86-64 execution uses Windows 11 ARM64 emulation and is functional
  package evidence, not physical-x86-64 performance evidence.

All runtime-owned artifact, release, SPDX, notice and license records are
preserved in each connector package. Public registry publication and production
signing are separate actions.

