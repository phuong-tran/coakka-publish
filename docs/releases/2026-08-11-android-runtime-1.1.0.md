# Android Runtime Connector 1.1.0 Candidate

Android connector `1.1.0` is staged under
`maven/android/releases/1.1.0+345e97b2/` as an AAR, sources JAR, POM, Gradle
module metadata, manifest, checksums, and consuming guide.

The AAR carries native runtime generation `2.3.0+345e97b2` and both
`arm64-v8a` and `x86_64` runtime/JNI pairs. Host-side build, unit-test, lint,
package-content, intake, and public-surface gates cover the packaged file. The
AAR records a clean source tree and has SHA-256
`3ce799885322c9ac92664bf028591bc77432960e7b2d85ecbd3c4e73362bf3cb`.
The audited connector handles nonblocking partial pipe I/O with cancellation,
provides a blocking monitor wait, and keeps explicit listener policy independent
of route metadata.

This is a candidate, not a current supported package. It is deliberately
absent from `artifacts/public-artifacts.tsv` and the checked-in Maven layout
because no matching Android device or emulator execution is recorded. Its
release-local `RELEASE.md` defines the exact promotion gate.
