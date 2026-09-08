#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CoAkkaHttp::runtime" for configuration "Release"
set_property(TARGET CoAkkaHttp::runtime APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(CoAkkaHttp::runtime PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/coakka_http_runtime.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/coakka_http_runtime.dll"
  )

list(APPEND _cmake_import_check_targets CoAkkaHttp::runtime )
list(APPEND _cmake_import_check_files_for_CoAkkaHttp::runtime "${_IMPORT_PREFIX}/lib/coakka_http_runtime.lib" "${_IMPORT_PREFIX}/bin/coakka_http_runtime.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
