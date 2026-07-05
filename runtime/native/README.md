# CoAkka Runtime Native v2

This lane publishes the direct runtime v2 native C ABI archive.

Release lane:

```text
runtime/native/releases/<version+snapshot>/coakka-runtime-native-v2-<version>.tar.gz
```

The current package includes the runtime C ABI and the scanner-clean TCP frame
transport profile. Runtime JVM, Python, Node.js, Go, C#, Rust, Spring Boot,
and Quarkus artifacts are published against this native package version.

Public release rule:

- package users must not have to install `protobuf`, `absl`, `CAF`, `libuv`,
  or other ambient third-party native libraries
- consumer-facing package/docs must not expose backend implementation names
