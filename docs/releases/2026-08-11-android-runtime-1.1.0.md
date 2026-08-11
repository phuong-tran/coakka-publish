# Android Runtime Connector 1.1.0 Candidate

Android connector `1.1.0` is staged under
`runtime/android/releases/1.1.0+6516990e/` as an AAR, sources JAR, POM, Gradle
module metadata, manifest, checksums, and consuming guide.

The AAR carries native runtime generation `2.3.0+6516990e` and both
`arm64-v8a` and `x86_64` runtime/JNI pairs. Host-side build, unit-test, lint,
package-content, intake, and public-surface gates cover the packaged file.

This is a candidate, not a current supported package. It is deliberately
absent from `artifacts/public-artifacts.tsv` and the checked-in Maven layout
because no matching Android device or emulator execution is recorded. Its
release-local `RELEASE.md` defines the exact promotion gate.
