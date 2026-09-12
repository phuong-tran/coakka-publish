# Third-Party Notices

CoAkka HTTP Core `1.0.0` statically incorporates the components below. Their
license texts are preserved under `third-party-licenses/`. These notices do not
change the license of CoAkka-owned material.

| Component | Frozen identity | License files |
| --- | --- | --- |
| libuv | `1.52.1`, `1cfa32ff59c076ffb6ed735bbc8c18361558661f` | `libuv/LICENSE`, `LICENSE-docs`, `LICENSE-extra` |
| uSockets | `86097c490263ab662d62e8e7b541390bdec7d149` | `usockets/LICENSE` |
| CoAkka uWebSockets fork | `20.79.0-coakka-http1.7`, `1542e0730d5942d9359d765c98689b4c80b8a223` | `uwebsockets/LICENSE` |
| Protobuf | `6.34.0`, `ee9f0bccf0950e07070e43d8d53ca70876fa050a` | `protobuf/LICENSE` |
| Abseil | `20250512.1`, `76bb24329e8bf5f39704eb10d21b9a80befa7c81` | `abseil/LICENSE` |
| utf8_range | Protobuf submodule at the frozen Protobuf commit | `utf8-range/LICENSE` |
| Boost.URL | Boost `1.91.0`, `1a80576db6b70828803819fb6925132193bc5d0e`; URL `000476c66a10efd723e22138135f9c8d6f713a31` | `boost/LICENSE_1_0.txt` |
| GCC support and C++ runtime | GCC Toolset `14.2.1`; Linux images only | `gcc-runtime/COPYING`, `COPYING.RUNTIME` |

CoAkka Commons `0.1.0` at
`292b502f2731a6ca1aa20822061d0dbd2328f32f` is CoAkka-owned material and is
covered by `NATIVE-LICENSE.md`, not a third-party license.

OpenSSL, zlib, nghttp2, ngtcp2, nghttp3, curl and c-ares are not compiled into
these `1.0.0` images. Their optional ABI operations fail closed when the loaded
Core does not advertise the corresponding feature bit.
