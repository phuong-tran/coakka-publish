# CoAkka HTTP for Kotlin/JVM 1.0.0

`coakka-http-jvm-1.0.0.jar` contains the Kotlin/Java connector and exact
JNI/Core pairs for all five supported targets. Its SHA-256 is
`9ffb0c79271e935081d237816a7703cf4deb8a558bd1ecf56615475e082992c7`.

The public API has complete KDoc. `ServiceBuilder`/`Service` supplies buffered
request/reply, while `CoreConfiguration`/`HttpCore` projects every ABI revision
2 capability. Core remains the sole HTTP resource owner.

This is a direct GitHub JAR distribution; no Maven repository coordinate is
published. Read [CONSUMING.md](CONSUMING.md) for classpath use.
