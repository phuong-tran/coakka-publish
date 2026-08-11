# CoAkka SFTP Artifact Publisher Native Addon

This template defines the archive consumed by native app hosts after an SFTP
addon release is promoted. It is not itself a released artifact.

## Archive Contents

```text
include/coakka/addons/artifact_publisher_sftp.h
native/<platform>/libcoakka_addon_artifact_publisher_sftp.<suffix>
cmake/CoAkkaRuntimeAddonArtifactPublisherSftpConfig.cmake
share/coakka/runtime-addons/artifact-publisher-sftp/addon.manifest.json
LICENSE.md
THIRD_PARTY_NOTICES.md
```

The archive contains the addon only. Consumers provide a compatible CoAkka
Runtime native package separately. Libssh2 and its target crypto/compression
closure are statically absorbed into the addon module and recorded in the
release manifest and third-party notices.

Only platforms listed in the release manifest may be present. Every listed
platform must have matching-host runtime-module and dynamic-dependency evidence.

See [`CONSUMING.md`](CONSUMING.md) for the native CMake integration shape.
