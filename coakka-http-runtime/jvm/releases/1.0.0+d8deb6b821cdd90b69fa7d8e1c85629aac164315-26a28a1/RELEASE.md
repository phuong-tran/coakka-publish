# JVM Release Evidence

- Source: `coakka-http-runtime-connector@26a28a1c4af3eb65334993532b4eaa082fc75438`.
- Gradle `check`, installed-package tests, KDoc verification, KDoc parser
  positive/negative fixtures, native manifest verification and the exact
  five-target `releaseJar` gate pass.
- The same aggregate JAR passes an installed Java smoke on macOS arm64, Linux
  arm64, Linux x86-64, Windows arm64 and Windows x86-64.
- ARM64 Windows uses a true ARM64 Microsoft JDK; x64 uses a true x64 JDK under
  Windows 11 ARM64's emulation layer.
- Each JNI image has the required ISA and 52 JNI exports and imports only its
  adjacent Core plus platform/JVM system libraries.

No Maven coordinate, signing or portable performance claim is included.
