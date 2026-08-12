# SwiftPM Runtime 2.4.0

Date: 2026-08-12

The public SwiftPM runtime coordinate is now:

```text
https://github.com/phuong-tran/coakka-runtime-swift.git
exact: 2.4.0
tag: v2.4.0
commit: f83a5e460286c976d34d1c93768116773904e748
native generation: 2.4.0+c2f53117
```

This release adds explicit embedded, outbound-only, and network-node startup
modes. Embedded and outbound-only runtimes do not allocate a TCP listener and
report local port `0`.

The tag bundles exact native payloads for macOS ARM64, Linux ARM64/x86-64, and
Windows ARM64/x86-64. Native payload verification, `swift test`, runtime and
transport smokes, package readiness, and a clean request/reply consumer cloned
from the remote tag pass on macOS ARM64. Payload presence is not matching-host
Swift execution evidence for Linux or Windows.

Publisher signing remains absent. The annotated tag is used by SwiftPM, and
the matching GitHub Release object marks `v2.4.0` as the latest release. No
duplicate binary assets are attached because the package already embeds the
platform payloads.
