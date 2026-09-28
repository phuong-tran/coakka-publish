# Consuming The JVM JAR

Add `coakka-http-jvm-1.0.0.jar` and Kotlin standard library 2.3.10 to the
application classpath. The package targets Java 8 bytecode. Do not extract or
replace native resources manually; the loader selects and verifies the matching
runtime and JNI library pair.

The JAR includes ProGuard/R8 consumer rules for the JNI lookup boundary.

