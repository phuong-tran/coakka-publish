#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CoAkkaHttp::host" for configuration "Release"
set_property(TARGET CoAkkaHttp::host APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(CoAkkaHttp::host PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/coakka_http_host.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/coakka_http_host.dll"
  )

list(APPEND _cmake_import_check_targets CoAkkaHttp::host )
list(APPEND _cmake_import_check_files_for_CoAkkaHttp::host "${_IMPORT_PREFIX}/lib/coakka_http_host.lib" "${_IMPORT_PREFIX}/bin/coakka_http_host.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
