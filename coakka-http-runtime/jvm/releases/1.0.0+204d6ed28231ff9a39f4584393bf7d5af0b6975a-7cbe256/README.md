# CoAkka HTTP For JVM 1.0.0

The JAR contains Java 8-compatible public classes plus exact runtime and JNI library pairs for
all five release targets. Runtime selection verifies target, runtime digest, and JNI
digest before loading.

Java and Kotlin share the same `ServiceBuilder` host-inline surface and the
complete typed `HttpCore` surface. No separately installed native runtime is
required.

