# Third-Party Notices

CoAkka HTTP Runtime `1.0.0` for `linux-x86_64`
statically incorporates the components below. The exact license payloads are
installed under `share/licenses/coakka-http-runtime/third-party/`. These
notices do not change the file-scoped license of CoAkka-owned material.

| Component | Version | Frozen identity | SPDX license | License payload |
| --- | --- | --- | --- | --- |
| libuv | `1.52.1` | `1cfa32ff59c076ffb6ed735bbc8c18361558661f` | `MIT` | `third-party/libuv/LICENSE` |
| uSockets | `git` | `86097c490263ab662d62e8e7b541390bdec7d149` | `Apache-2.0` | `third-party/usockets/LICENSE` |
| uWebSockets | `20.79.0-coakka-http1.7` | `1542e0730d5942d9359d765c98689b4c80b8a223` | `Apache-2.0` | `third-party/uwebsockets/LICENSE` |
| Boost.URL | `1.91.0` | `1a80576db6b70828803819fb6925132193bc5d0e, URL 000476c66a10efd723e22138135f9c8d6f713a31` | `BSL-1.0` | `third-party/boost/LICENSE_1_0.txt` |
| Protobuf | `6.34.0` | `ee9f0bccf0950e07070e43d8d53ca70876fa050a` | `BSD-3-Clause` | `third-party/protobuf/LICENSE` |
| Abseil | `20250512.1` | `76bb24329e8bf5f39704eb10d21b9a80befa7c81` | `Apache-2.0` | `third-party/abseil/LICENSE` |
| utf8_range | `protobuf-submodule` | `ee9f0bccf0950e07070e43d8d53ca70876fa050a` | `MIT` | `third-party/utf8-range/LICENSE` |
| OpenSSL | `3.5.8` | `sha256:a8f84a39918ec6415ce765d9b429d313ba97b8143169c172e734b9514464f5b2` | `Apache-2.0` | `third-party/openssl/LICENSE.txt` |
| curl | `8.21.0` | `sha256:aa1b66a70eace83dc624508745646c08ae561de512ab403adffb93ac87fc72e6` | `curl` | `third-party/curl/COPYING` |
| c-ares | `1.34.8` | `sha256:c222b6d681096f9444d2c4863d2c1174019e27cacca0a4a5c114d36dd7d7bf78` | `MIT` | `third-party/c-ares/LICENSE.md` |
| nghttp2 | `1.70.0` | `85e300c79fb6dbcfa9c1013215c8710c1c2cd3d2` | `MIT` | `third-party/nghttp2/COPYING` |
| ngtcp2 | `1.25.0` | `f9e9ff01ad2c8116bc09de4f644b0028a61486a6` | `MIT` | `third-party/ngtcp2/COPYING` |
| nghttp3 | `1.18.0` | `dbfc24286138cb0b6490160e7ca87fe1ce6722a0` | `MIT` | `third-party/nghttp3/COPYING` |
| sfparse | `git` | `4b313cfd2e1b389ae632b36dcd50402307289af2` | `MIT` | `third-party/sfparse/COPYING` |


CoAkka Commons `0.1.0` at
`292b502f2731a6ca1aa20822061d0dbd2328f32f` is CoAkka-owned material. Compiled CoAkka
native artifacts and their native-only provenance are covered by the CoAkka
Native Artifact License 1.2 included with this package.

On Linux, `liburing.so.2` is an optional runtime-owned runtime discovery boundary.
It is not bundled or directly linked. Provider absence is reported through
runtime info and preserves the bounded platform fallback law.

Source revision: `204d6ed28231ff9a39f4584393bf7d5af0b6975a`.
