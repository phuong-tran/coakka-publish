# CoAkka HTTP Runtime 1.0.0 Native Assembly

This directory is the native assembly for one source revision. Its presence is
not a publication or a release-readiness claim. Read [RELEASE.md](RELEASE.md)
for the current gate state, and distribute it only after
`scripts/verify-http-runtime-release.py` passes from the `coakka-publish`
repository root.

Each completed target directory contains the shared library, the focused
`include/coakka/http/host.h` C header, CMake package metadata, and required
license and notice files. C++ applications use the same C header. Language
connector source and registry-specific packages are not included here.

| Target directory | Operating system and architecture |
| --- | --- |
| `macos-aarch64` | macOS ARM64 |
| `linux-aarch64` | Linux ARM64 |
| `linux-x86_64` | Linux x86-64 |
| `windows-aarch64` | Windows ARM64 |
| `windows-x86_64` | Windows x86-64 |

Select only the directory matching the deployment machine. In a CMake
consumer, add that directory to `CMAKE_PREFIX_PATH` and link the imported
target:

```cmake
find_package(CoAkkaHttpHost 1.0.0 EXACT REQUIRED CONFIG)
target_link_libraries(your_application PRIVATE CoAkka::HttpHost)
```

Do not combine files from different targets. The final `manifest.json` and
`SHA256SUMS` identify the reviewed assembly; neither is a substitute for the
matching-host execution and benchmark gates recorded in `RELEASE.md`.
