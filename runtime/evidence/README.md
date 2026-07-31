# Runtime Evidence Artifacts

This directory contains optional evidence artifacts for CoAkka Runtime.

Evidence artifacts are convenience runners, not primary connector packages,
cross-machine benchmarks, or production SLO claims. Source-first samples and
the canonical measurement contract live in `coakka-samples`. Published
artifacts here let users execute the same public-ABI harness without installing
a native toolchain.

Current lane:

- `native`: prebuilt runner for native runtime smoke, pressure, stress, and soak
  evidence over a complete local target request/reply path.
