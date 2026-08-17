# SwiftPM macOS 13 Payload Correction

Date: 2026-08-17

The public SwiftPM coordinates are now:

```text
Runtime: https://github.com/phuong-tran/coakka-runtime-swift.git
exact: 2.4.1
tag: v2.4.1
commit: 8b94a6bb7962bd694d81ee90469aca08358ff156
native generation: 2.4.0+c2f53117

Logger: https://github.com/phuong-tran/coakka-logger-swift.git
exact: 1.2.2
tag: v1.2.2
commit: 8dd590874af61a1d08de55e86d340fb35d1caffb
native generation: 1.2.1+f50756ebff0d
```

Both patch releases replace only the bundled macOS ARM64 dylib. Runtime was
rebuilt from exact native snapshot
`c2f53117f991f67f809a0bf46bac2ce26091eb78`; Logger was rebuilt from exact
native snapshot `f50756ebff0d2635243093baf533993c4d9fe9dc`. Both builds use deployment
target `13.0`. Swift tools remain `6.0` for Runtime and `5.9` for Logger; public
Swift API, native ABI, native generations, and the Runtime Linux/Windows
payloads are unchanged.

Corrected SHA-256 values:

```text
956f6cf04c18a923cc6416366b1a1ee1e5cae67e6f61bf3988e6bbeb09db6a7c  libcoakka_runtime_v2.dylib
28db676f3a56ea67c909697d82453edd76372bd8c9916dd43250f453cfedc87e  libcoakka_logger_core.10.dylib
```

Evidence on Apple Swift `6.2`, macOS ARM64:

- Runtime native verification, seven package tests, focused File Lane and
  Stream Lane tests, runtime and transport smokes, package readiness, and a
  clean consumer cloned from remote tag `v2.4.1` pass.
- Logger native verification, two package tests, logger smoke, and a clean
  consumer cloned from remote tag `v1.2.2` pass.
- Public GitHub Actions runs `32043120029` and `32043116248` pass on the exact
  release commits.
- `xcrun vtool -show-build` reports `minos 13.0` for both dylibs.

The Mach-O metadata establishes the binary deployment floor. These runs used
a newer macOS ARM64 host and do not claim execution on a macOS 13 host. The
annotated tags and matching GitHub Release objects carry no duplicate assets
because SwiftPM already embeds the native payloads.
