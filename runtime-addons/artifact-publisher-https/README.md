# HTTPS Artifact Publisher Runtime Addon

Status: native `1.1.0+d1032f6d` is public for `linux-aarch64`, `linux-x86_64`, `macos-aarch64`, `windows-aarch64`, `windows-x86_64`. It requires CoAkka Runtime
native `2.4.0` or newer with File Lane support.

Fetches one immutable HTTPS object, verifies size and SHA-256, stages without replacement, and distributes it through File Lane.

## Native Coordinate

```text
runtime-addons/artifact-publisher-https/native/releases/1.1.0+d1032f6d/
  coakka-runtime-addon-artifact-publisher-https-native-1.1.0.tar.gz
```

The application owns credentials, authorization grants, lifecycle, retry and
rollout policy. The addon owns protocol mechanics, bounded acquisition,
integrity checking, and no-clobber staging. Runtime owns File Lane delivery.

This release exposes the reviewed C ABI. It does not yet provide a Go, Swift,
JVM, Node, Python, or .NET connector; that wrapper layer is a separate product
slice and is not implied by availability of this native archive.
