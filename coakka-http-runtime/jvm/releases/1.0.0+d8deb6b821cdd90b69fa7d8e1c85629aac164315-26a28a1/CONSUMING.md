# Consuming The JVM JAR

Add the downloaded JAR directly to the application compile and runtime
classpath. Java example:

```sh
javac -cp /path/to/coakka-http-jvm-1.0.0.jar App.java
java -cp /path/to/coakka-http-jvm-1.0.0.jar:. App
```

The JAR targets Java 8 bytecode and was built with JDK 17. The loader selects
the native pair from the process OS and ISA, checks bounded resource names,
sizes and SHA-256 values, then loads the adjacent Core before JNI. Windows x64
processes on Windows ARM64 select x64 resources rather than the host ISA.

Close `Service`, `CoreConfiguration` and `HttpCore` deterministically. The
buffered service requires cooperative handlers and must not be closed by one
of its own handlers. Each low-level event lane permits one reader.
