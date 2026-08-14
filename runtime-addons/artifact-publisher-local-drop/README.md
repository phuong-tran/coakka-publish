# Local Drop Artifact Publisher Runtime Addon

Status: native `1.1.0+d1032f6d` is public for `linux-aarch64`, `linux-x86_64`, `macos-aarch64`. It requires CoAkka Runtime
native `2.4.0` or newer with File Lane support.

Acquires one stable file from an anchored local drop directory, verifies integrity, and distributes it through File Lane.

## Native Coordinate

```text
runtime-addons/artifact-publisher-local-drop/native/releases/1.1.0+d1032f6d/
  coakka-runtime-addon-artifact-publisher-local-drop-native-1.1.0.tar.gz
```

The application owns credentials, authorization grants, lifecycle, retry and
rollout policy. The addon owns protocol mechanics, bounded acquisition,
integrity checking, and no-clobber staging. Runtime owns File Lane delivery.

This release exposes the reviewed C ABI. It does not yet provide a Go, Swift,
JVM, Node, Python, or .NET connector; that wrapper layer is a separate product
slice and is not implied by availability of this native archive.
