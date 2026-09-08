cmake_minimum_required(VERSION 3.20)

project(CoAkkaHttpRuntimeTest LANGUAGES C)

include(CTest)
find_package(Threads REQUIRED)
find_package(CoAkkaHttpCore 1.1.0 EXACT REQUIRED CONFIG)

option(COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS
       "Treat native test compiler warnings as errors" ON)
option(COAKKA_HTTP_RUNTIME_TEST_ENABLE_ASAN_UBSAN
       "Instrument the native test harness with address and undefined checks" OFF)
option(COAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN
       "Instrument the native test harness with thread checks" OFF)

if(COAKKA_HTTP_RUNTIME_TEST_ENABLE_ASAN_UBSAN AND
   COAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN)
  message(FATAL_ERROR "Address/undefined and thread checks require separate builds")
endif()

set(thread_source test_threads_posix.c)
if(WIN32)
  set(thread_source test_threads_windows.c)
endif()

add_executable(
    coakka_http_runtime_test
    core_abi.c
    test_support.c
    "${thread_source}")
set_target_properties(
    coakka_http_runtime_test
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO)
target_link_libraries(
    coakka_http_runtime_test
    PRIVATE CoAkkaHttp::core Threads::Threads)

if(MSVC)
  target_compile_options(
      coakka_http_runtime_test
      PRIVATE /W4 $<$<BOOL:${COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(
      coakka_http_runtime_test
      PRIVATE
          -Wall
          -Wextra
          -Wpedantic
          -Wconversion
          -Wshadow
          -Wsign-conversion
          -Wstrict-prototypes
          $<$<BOOL:${COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

if(COAKKA_HTTP_RUNTIME_TEST_ENABLE_ASAN_UBSAN OR
   COAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    message(FATAL_ERROR "Native test instrumentation requires Clang or GCC")
  endif()
  if(COAKKA_HTTP_RUNTIME_TEST_ENABLE_ASAN_UBSAN)
    set(sanitizers address,undefined)
  else()
    set(sanitizers thread)
  endif()
  target_compile_options(
      coakka_http_runtime_test PRIVATE -fno-omit-frame-pointer
                                       -fno-sanitize-recover=all
                                       "-fsanitize=${sanitizers}")
  target_link_options(
      coakka_http_runtime_test PRIVATE "-fsanitize=${sanitizers}")
endif()

add_test(NAME coakka_http_runtime_test COMMAND coakka_http_runtime_test)
set_tests_properties(coakka_http_runtime_test PROPERTIES TIMEOUT 180)
