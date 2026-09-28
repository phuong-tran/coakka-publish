# CoAkka HTTP Runtime Native 1.0.0

This immutable release contains the installed ABI 8 SDK for all five supported
targets. Select exactly one target tree for the process operating system and
CPU.

| Target | Runtime SHA-256 |
| --- | --- |
| `macos-aarch64` | `027bcabeb06bd7b2810b33a015853c205600466f998816860ebc08115ed4331e` |
| `linux-aarch64` | `22a7bfe646699c5770ca1b83407efa65df4222bbd758dd9bbe00a462108a35f2` |
| `linux-x86_64` | `0f5208841f331c62dc10cb55e776a161093ead8e01aaa7a3ef0c353bf3a77f79` |
| `windows-aarch64` | `9bc1f29ee84baa23e713258da2f922f9a7334bfd1f2a049901841078288b6990` |
| `windows-x86_64` | `94d9ef20d45eadba479dfecf50cdcb773b1aa764524c7d901299b0d9aabba0bc` |

Every tree installs only `coakka/http/http.h`, the closed shared library and
relocatable CMake metadata. The public header SHA-256 is
`c9072f66c466a56d44784cf3f3c1054f796ebf44bc431d06ece9fed7358a9bd2`;
the library exposes exactly the 176 names in `ABI-EXPORTS.txt`.

