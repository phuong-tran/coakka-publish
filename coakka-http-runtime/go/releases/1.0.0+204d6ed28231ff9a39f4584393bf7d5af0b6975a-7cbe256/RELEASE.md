# Go Release Evidence

- Source: `coakka-http-runtime-connector@7cbe25630e73750f13e477999b6fca2e3ced1541`.
- The archive was built twice byte-identically and then extracted independently.
- Test, vet and 20 repetitions pass on all five targets.
- Race passes on macOS, Linux x86-64, native Linux ARM64 evidence and Windows
  x86-64. Go does not support race on Windows ARM64; the Pi race runtime cannot
  start with its 47-bit VMA layout.
- Real HTTP/2, HTTP/3, TLS/mTLS, static/SPA/application files, outbound,
  monitoring and shutdown are covered by the qualified suites.

