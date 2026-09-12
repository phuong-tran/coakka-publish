# CoAkka HTTP Core Native 1.0.0

This immutable release contains installed SDK trees for all five supported
targets. Use only the tree matching the process operating system and CPU.

| Target | Core SHA-256 |
| --- | --- |
| `macos-aarch64` | `ee6e602692f71905b76f90205ad020409d667d429cd31bba29d62f8d4c60a009` |
| `linux-aarch64` | `ece8a618e763c10e23edea90bdd5fe968bf448fd5186b8c05d47f025f76b912b` |
| `linux-x86_64` | `eaa502ed5ccd0398ffdd463a6930f38def4796e8cd87b81c9cb8c3d847c959e6` |
| `windows-aarch64` | `1378347cb9ff27fee165e0711980ce2d8695f0c92a7ec61479788fedbc3ad043` |
| `windows-x86_64` | `1bf2220891e35893b8d47725345ba3bc4740d532eac288b022f2d0f52cdc6e69` |

Every tree carries the same public header, SHA-256
`ad95d1369b9851eb3a5299a9b7c108e3a3628b87347c3915233976fbda456a6b`.
ABI revision 2 has exactly 125 allow-listed C exports. The shared library owns
all HTTP resources and statically contains every non-OS dependency.

Read [CONSUMING.md](CONSUMING.md) before linking and [RELEASE.md](RELEASE.md)
for evidence and limitations.
