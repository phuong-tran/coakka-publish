include_guard(GLOBAL)

get_filename_component(_coakka_runtime_native_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
    set(_coakka_runtime_native_platform "macos-aarch64")
    set(_coakka_runtime_native_lib_name "libcoakka_runtime_v2.dylib")
  else()
    message(FATAL_ERROR "CoAkka runtime native package does not include macOS platform: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(_coakka_runtime_native_platform "linux-aarch64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    set(_coakka_runtime_native_platform "linux-x86_64")
  else()
    message(FATAL_ERROR "CoAkka runtime native package does not include Linux platform: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
  set(_coakka_runtime_native_lib_name "libcoakka_runtime_v2.so")
else()
  message(FATAL_ERROR "CoAkka runtime native package does not include system: ${CMAKE_SYSTEM_NAME}")
endif()

set(_coakka_runtime_native_include_dir "${_coakka_runtime_native_root}/include")
set(_coakka_runtime_native_lib_dir "${_coakka_runtime_native_root}/native/${_coakka_runtime_native_platform}")
set(_coakka_runtime_native_lib "${_coakka_runtime_native_lib_dir}/${_coakka_runtime_native_lib_name}")

if(NOT EXISTS "${_coakka_runtime_native_lib}")
  message(FATAL_ERROR "CoAkka runtime native library is missing: ${_coakka_runtime_native_lib}")
endif()

if(NOT TARGET CoAkkaRuntimeNativeV2::runtime_v2)
  add_library(CoAkkaRuntimeNativeV2::runtime_v2 SHARED IMPORTED)
  set_target_properties(CoAkkaRuntimeNativeV2::runtime_v2 PROPERTIES
    IMPORTED_LOCATION "${_coakka_runtime_native_lib}"
    INTERFACE_INCLUDE_DIRECTORIES "${_coakka_runtime_native_include_dir}"
  )
endif()

set(CoAkkaRuntimeNativeV2_FOUND TRUE)
set(CoAkkaRuntimeNativeV2_PLATFORM "${_coakka_runtime_native_platform}")
set(CoAkkaRuntimeNativeV2_NATIVE_LIBRARY "${_coakka_runtime_native_lib}")
