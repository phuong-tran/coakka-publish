# JVM Release Evidence

- Source: `coakka-http-runtime-connector@7cbe25630e73750f13e477999b6fca2e3ced1541`.
- Two five-target JAR builds are byte-identical.
- Source and packaged suites pass 35/35 on the qualified Unix targets.
- The exact aggregate JAR passes five real loopback smokes on each of the five
  targets, including Java 8 on the physical Raspberry Pi.
- 137 classes retain Java 8 bytecode; strict Kotlin/KDoc, native package
  validation and ProGuard JNI smoke pass.

