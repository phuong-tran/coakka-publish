# CoAkka HTTP Runtime for Java and Kotlin changelog

## 2.3.1

Core generation **2.3**, native SDK **2.3.0**, C ABI **10**. The final `1`
is the first JVM connector revision on this Core generation.

- Add `MultiProtocolListener` for a buffered HTTP/1.1, HTTP/2 and HTTP/3
  endpoint on one TLS port. `multiProtocolListenerInfo()` reports Core's
  accepted address, port, protocol and transport state.
- Retain DATA/TRAILERS credit until the Service request body consumes or
  discards the copied content. The existing reader returns each Core lease
  exactly once, including cancellation and close.
- Adopt the qualified Core corrections for upload and stream lifetime,
  HTTP/3 response streaming/SSE, drain, timeout wakeups, and Windows
  UDP/timer behavior. Standalone HTTP/2 and HTTP/3 WebSocket use Extended
  CONNECT. Use standalone profiles for streaming, SSE, WebSocket, mTLS
  and live route/TLS control.
- Refresh the Java and Kotlin examples, Maven and Gradle installation,
  version explanation, ecosystem/benchmark links and GitHub Sponsors/Ko-fi.

The library remains compatible with Java 8. README applications use JDK 17
and Kotlin 2.3.10. Keep the managed JAR and native classifier on version
2.3.1; they use native SDK 2.3.0. JNI and the shrunk managed implementation
are identical to the qualified 2.3.1 candidate.

## 2.2.1

First JVM revision on Core generation 2.2, SDK 2.2.0 and ABI 10. Preserve the
Java 8 library target and the selected one-loop integration. Adopt the
standalone HTTP/3 wakeup corrections.

[GitHub Sponsors](https://github.com/sponsors/phuong-tran) ·
[Ko-fi](https://ko-fi.com/phuongnamtran)
