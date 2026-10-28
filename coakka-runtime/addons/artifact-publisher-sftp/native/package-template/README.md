# CoAkka SFTP Artifact Publisher Native Addon

This template defines the archive consumed by native app hosts after an SFTP
addon release is promoted. It is not itself a released artifact.

## Archive Contents

```text
include/coakka/addons/artifact_publisher_sftp.h
native/<platform>/libcoakka_addon_artifact_publisher_sftp.<suffix>
native/<platform>/libcoakka_addon_artifact_publisher_sftp.<loader-suffix>
native/windows-<arch>/coakka_addon_artifact_publisher_sftp.lib
cmake/CoAkkaRuntimeAddonArtifactPublisherSftpConfig.cmake
share/coakka/runtime-addons/artifact-publisher-sftp/addon.manifest.json
LICENSE.md
THIRD_PARTY_NOTICES.md
```

The archive contains the addon only. Consumers provide a compatible CoAkka
Runtime native package separately. Libssh2 and its target crypto/compression
closure are statically absorbed into the addon module and recorded in the
release manifest and third-party notices.

POSIX packages carry both the unversioned link-time filename and the regular
loader-facing `SOVERSION` filename (`.so.0` or `.0.dylib`). Archive intake
rejects a package that omits either file; symlinks are not accepted inside the
public archive.

Windows packages carry the architecture-matched DLL plus its MSVC import
library. The separately supplied Runtime package owns its own DLL and link
metadata; the addon archive never bundles a Runtime implementation.
Release DLLs are not Authenticode-signed; verify the published SHA-256 and apply
an organization signature after verification when local application-control
policy requires one.

Only platforms listed in the release manifest may be present. Every listed
platform must have matching-host runtime-module and dynamic-dependency evidence.
Linux addon mappings remain resident until process exit because the packaged
ELF modules carry `DF_1_NODELETE`; publisher instances still support normal
stop and destroy.

See [`CONSUMING.md`](CONSUMING.md) for the native CMake integration shape.
