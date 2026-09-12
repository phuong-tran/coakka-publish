# Native Release Evidence

- Source: `coakka-http-runtime@d8deb6b821cdd90b69fa7d8e1c85629aac164315`.
- Target matrix: macOS arm64, Linux arm64/x86-64, Windows arm64/x86-64.
- Artifact checks: ISA, 125-symbol export allow-list, identical public header,
  OS-only dynamic imports, no private header/schema/source path.
- Linux: GCC Toolset 14.2.1 producer on GLIBC 2.28; no required GLIBC symbol
  newer than 2.28; compiler support and C++ runtime are statically linked.
- Windows: MSVC ARM64/x64, static CRT and system-DLL-only imports.
- Installed evidence: five public black-box C tests pass five times on each
  target through only the installed SDK tree and real loopback HTTP.
- Sanitizers: separate matching-host macOS ASan/LSan, TSan and halt-on-error
  UBSan graphs pass 156/156 each without suppression.
- Static analysis: Cppcheck 2.20 passes the public projection units; Clang's C
  analyzer passes the installed consumer. LLVM clang-tidy cannot consume the
  AppleClang SDK compile database and is not claimed.

Windows x86-64 execution uses Windows 11 ARM64's x64 emulation layer. This is
functional/package evidence, not a physical-host or performance claim. Signing,
SBOM output, alternate libc, macOS x86-64 and portable performance remain out
of scope.
