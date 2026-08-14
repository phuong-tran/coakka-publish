# SFTP Artifact Publisher Runtime Addon

Status: native `1.2.0+88b9a047` is public for `linux-aarch64`, `linux-x86_64`, `macos-aarch64`, `windows-aarch64`, `windows-x86_64`. It requires CoAkka Runtime
native `2.3.0` or newer with File Lane support.

Fetches one pinned SFTP object, verifies size and SHA-256, stages without replacement, and distributes it through File Lane.

## Native Coordinate

```text
runtime-addons/artifact-publisher-sftp/native/releases/1.2.0+88b9a047/
  coakka-runtime-addon-artifact-publisher-sftp-native-1.2.0.tar.gz
```

The application owns credentials, authorization grants, lifecycle, retry and
rollout policy. The addon owns protocol mechanics, bounded acquisition,
integrity checking, and no-clobber staging. Runtime owns File Lane delivery.

This release exposes the reviewed C ABI. It does not yet provide a Go, Swift,
JVM, Node, Python, or .NET connector; that wrapper layer is a separate product
slice and is not implied by availability of this native archive.
