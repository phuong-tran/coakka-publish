include_guard(GLOBAL)

include(CMakeFindDependencyMacro)
find_dependency(CoAkkaRuntimeNativeV2 CONFIG)

get_filename_component(
  _coakka_sftp_addon_root
  "${CMAKE_CURRENT_LIST_DIR}/.."
  ABSOLUTE
)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND
   CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
  set(_coakka_sftp_addon_platform "macos-aarch64")
  set(_coakka_sftp_addon_library_name
      "libcoakka_addon_artifact_publisher_sftp.dylib")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(_coakka_sftp_addon_platform "linux-aarch64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    set(_coakka_sftp_addon_platform "linux-x86_64")
  else()
    message(FATAL_ERROR
      "CoAkka SFTP addon package does not include Linux platform: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
  set(_coakka_sftp_addon_library_name
      "libcoakka_addon_artifact_publisher_sftp.so")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    set(_coakka_sftp_addon_platform "windows-aarch64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    set(_coakka_sftp_addon_platform "windows-x86_64")
  else()
    message(FATAL_ERROR
      "CoAkka SFTP addon package does not include Windows platform: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
  set(_coakka_sftp_addon_library_name
      "libcoakka_addon_artifact_publisher_sftp.dll")
  set(_coakka_sftp_addon_implib_name
      "libcoakka_addon_artifact_publisher_sftp.dll.a")
  set(_coakka_sftp_runtime_implib_name
      "libcoakka_runtime_v2.dll.a")
else()
  message(FATAL_ERROR
    "CoAkka SFTP addon package does not include host: ${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(_coakka_sftp_addon_include_dir
    "${_coakka_sftp_addon_root}/include")
set(_coakka_sftp_addon_library
    "${_coakka_sftp_addon_root}/native/${_coakka_sftp_addon_platform}/${_coakka_sftp_addon_library_name}")

if(NOT EXISTS "${_coakka_sftp_addon_library}")
  message(FATAL_ERROR
    "CoAkka SFTP addon native library is missing: ${_coakka_sftp_addon_library}")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(_coakka_sftp_addon_implib
      "${_coakka_sftp_addon_root}/native/${_coakka_sftp_addon_platform}/${_coakka_sftp_addon_implib_name}")
  set(_coakka_sftp_runtime_implib
      "${_coakka_sftp_addon_root}/native/${_coakka_sftp_addon_platform}/${_coakka_sftp_runtime_implib_name}")
  if(NOT EXISTS "${_coakka_sftp_addon_implib}" OR
     NOT EXISTS "${_coakka_sftp_runtime_implib}")
    message(FATAL_ERROR
      "CoAkka SFTP addon Windows import libraries are missing for ${_coakka_sftp_addon_platform}")
  endif()
  set_property(TARGET CoAkkaRuntimeNativeV2::runtime_v2 PROPERTY
    IMPORTED_IMPLIB "${_coakka_sftp_runtime_implib}")
endif()

if(NOT TARGET CoAkkaRuntimeAddonArtifactPublisherSftp::artifact_publisher_sftp)
  add_library(
    CoAkkaRuntimeAddonArtifactPublisherSftp::artifact_publisher_sftp
    SHARED IMPORTED
  )
  set_target_properties(
    CoAkkaRuntimeAddonArtifactPublisherSftp::artifact_publisher_sftp
    PROPERTIES
      IMPORTED_LOCATION "${_coakka_sftp_addon_library}"
      INTERFACE_INCLUDE_DIRECTORIES "${_coakka_sftp_addon_include_dir}"
      INTERFACE_LINK_LIBRARIES "CoAkkaRuntimeNativeV2::runtime_v2"
  )
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(
      TARGET CoAkkaRuntimeAddonArtifactPublisherSftp::artifact_publisher_sftp
      PROPERTY IMPORTED_IMPLIB "${_coakka_sftp_addon_implib}")
  endif()
endif()

set(CoAkkaRuntimeAddonArtifactPublisherSftp_FOUND TRUE)
set(CoAkkaRuntimeAddonArtifactPublisherSftp_PLATFORM
    "${_coakka_sftp_addon_platform}")
set(CoAkkaRuntimeAddonArtifactPublisherSftp_NATIVE_LIBRARY
    "${_coakka_sftp_addon_library}")
