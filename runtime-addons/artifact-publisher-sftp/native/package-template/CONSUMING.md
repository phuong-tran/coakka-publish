# Consuming The SFTP Artifact Publisher Addon

Unpack Runtime native `2.3.0` or newer and SFTP addon `0.1.0`. Expose both CMake
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
installed on the target host. Version `0.1.0` currently supports
`macos-aarch64` only.
