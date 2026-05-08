include_guard(GLOBAL)

get_filename_component(_coakka_logger_native_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
    set(_coakka_logger_native_platform "macos-aarch64")
    set(_coakka_logger_native_lib_name "libcoakka_logger_core.dylib")
  else()
    message(FATAL_ERROR "CoAkka logger native package does not include macOS platform: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(_coakka_logger_native_platform "linux-aarch64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    set(_coakka_logger_native_platform "linux-x86_64")
  else()
    message(FATAL_ERROR "CoAkka logger native package does not include Linux platform: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
  set(_coakka_logger_native_lib_name "libcoakka_logger_core.so")
else()
  message(FATAL_ERROR "CoAkka logger native package does not include system: ${CMAKE_SYSTEM_NAME}")
endif()

set(_coakka_logger_native_include_dir "${_coakka_logger_native_root}/include")
set(_coakka_logger_native_lib_dir "${_coakka_logger_native_root}/native/${_coakka_logger_native_platform}")
set(_coakka_logger_native_lib "${_coakka_logger_native_lib_dir}/${_coakka_logger_native_lib_name}")
set(_coakka_logger_native_cpp_src "${_coakka_logger_native_root}/src/native_cpp/Connector.cpp")

if(NOT EXISTS "${_coakka_logger_native_lib}")
  message(FATAL_ERROR "CoAkka logger native library is missing: ${_coakka_logger_native_lib}")
endif()

if(NOT TARGET CoAkkaLoggerNative::core)
  add_library(CoAkkaLoggerNative::core SHARED IMPORTED)
  set_target_properties(CoAkkaLoggerNative::core PROPERTIES
    IMPORTED_LOCATION "${_coakka_logger_native_lib}"
    INTERFACE_INCLUDE_DIRECTORIES "${_coakka_logger_native_include_dir}"
  )
endif()

if(NOT TARGET CoAkkaLoggerNative::native_cpp_connector)
  add_library(coakka_logger_native_cpp_connector STATIC "${_coakka_logger_native_cpp_src}")
  add_library(CoAkkaLoggerNative::native_cpp_connector ALIAS coakka_logger_native_cpp_connector)
  target_include_directories(coakka_logger_native_cpp_connector PUBLIC "${_coakka_logger_native_include_dir}")
  target_compile_features(coakka_logger_native_cpp_connector PUBLIC cxx_std_20)
  target_link_libraries(coakka_logger_native_cpp_connector PUBLIC CoAkkaLoggerNative::core)
endif()

set(CoAkkaLoggerNative_FOUND TRUE)
set(CoAkkaLoggerNative_PLATFORM "${_coakka_logger_native_platform}")
set(CoAkkaLoggerNative_NATIVE_LIBRARY "${_coakka_logger_native_lib}")
