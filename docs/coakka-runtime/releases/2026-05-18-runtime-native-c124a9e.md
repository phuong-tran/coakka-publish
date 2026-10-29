# CoAkka Runtime Native Refresh c124a9e

This note records the native runtime `0.2.0+c124a9e` public refresh.

| Surface | Artifact | Version |
| --- | --- | --- |
| Native C ABI | `runtime/native/releases/0.2.0+c124a9e/coakka-runtime-native-v2-0.2.0.tar.gz` | `0.2.0+c124a9e` |

The refresh updates the public native runtime package and root native libraries
after the cluster routing terminal-outcome coverage pass. Existing JVM, Python,
Node.js, Go, C#, Rust, Spring Boot, and Quarkus runtime packages remain on the
`0.2.0+94a5729-6b7a3bf` connector set until those lanes are refreshed.

Mojo and Zig are planned next connector lanes. They are listed as future
runtime language surfaces, not as public artifacts in this native-only refresh.

Validation covered the public native release path on Linux `aarch64`, Linux
`x86_64`, and macOS `aarch64`, public surface scanning, native dependency
checks, publish manifest verification, and the public native samples.
