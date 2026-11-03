# Runtime 3.0.0 addon validation binaries

Choose the archive for your platform and verify it against [SHA256SUMS](SHA256SUMS)
before extraction. Use the addon headers and libraries with CoAkka Runtime 3.0.0,
native ABI 1 and File Lane support.

| Platform | Archive | Validation |
| --- | --- | --- |
| Linux x64 | [linux-x86_64.tar.gz](linux-x86_64.tar.gz) | 12 workflows passed in WSL2 with Debian 12 and glibc 2.36 |
| Linux ARM64 | [linux-aarch64.tar.gz](linux-aarch64.tar.gz) | 12 workflows and 16,384 lifecycle cycles passed on Raspberry Pi 5. The Debian 12 run also passed |
| macOS ARM64 | [macos-aarch64.tar.gz](macos-aarch64.tar.gz) | 12 workflows and 34 UBSan checks passed on physical Mac |
| Windows x64 | [windows-x86_64-preview.zip](windows-x86_64-preview.zip) | **Preview.** Build and archive checks passed. Runtime workflows remain unverified because Windows blocked DLL loading |
| Windows ARM64 | [windows-aarch64-preview.zip](windows-aarch64-preview.zip) | **Preview.** 11 addon lifecycle checks passed in a Windows 11 ARM64 VM. Full provider workflows remain unverified |

The Windows archives contain 11 addons. Local Drop is available in the Linux
and macOS archives. All five archives include their applicable license texts.
The binary scan still reports dependency identifiers in 35 of the 58 native
images. The archives are Git validation snapshots built from source
`1af88117e2875f0ed80ed8277d66353d9fb6ebeb`.
