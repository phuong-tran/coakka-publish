# Consuming The SFTP Artifact Publisher Addon

Unpack Runtime native `2.3.0` or newer and SFTP addon `1.1.0`. Expose both CMake
package directories:

```sh
cmake -S app -B build \
  -DCoAkkaRuntimeNativeV2_DIR=/path/to/runtime/cmake \
  -DCoAkkaRuntimeAddonArtifactPublisherSftp_DIR=/path/to/addon/cmake
```

```cmake
find_package(CoAkkaRuntimeNativeV2 CONFIG REQUIRED)
find_package(CoAkkaRuntimeAddonArtifactPublisherSftp CONFIG REQUIRED)

add_executable(service_a main.c)
target_link_libraries(service_a PRIVATE
  CoAkkaRuntimeAddonArtifactPublisherSftp::artifact_publisher_sftp)
```

The imported addon target brings the compatible runtime target as an interface
dependency. The app host still owns Runtime and File Lane lifecycle and must
keep the borrowed sender lane alive until the publisher has stopped and been
destroyed.

Keep both matching platform native directories on the process loader path. No
SFTP, libssh2, OpenSSL, or other addon implementation package should be
installed on the target host. Version `1.1.0` supports `linux-aarch64`,
`linux-x86_64`, `macos-aarch64`, `windows-aarch64`, and `windows-x86_64`.

On Windows, place `libcoakka_runtime_v2.dll` and
`libcoakka_addon_artifact_publisher_sftp.dll` beside `service_a.exe`, or add
their matching native directories to `PATH`. Private-key authentication also
requires the companion OpenSSH public key at `<private-key>.pub`.

The release DLLs are not Authenticode-signed. SHA-256 validates the downloaded
bytes, but it is not a publisher identity. Standard Windows can load an
unsigned DLL; SmartScreen or a managed application-control policy may require
an organization-trusted signature.

The app must keep the Linux addon module loaded for process lifetime. Packaged
Linux modules carry `DF_1_NODELETE` because their static OpenSSL state is not a
supported hot-unload/reload boundary.
