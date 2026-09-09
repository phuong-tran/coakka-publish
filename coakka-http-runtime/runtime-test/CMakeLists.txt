cmake_minimum_required(VERSION 3.20)

project(CoAkkaHttpRuntimeTest LANGUAGES C CXX)

include(CTest)
find_package(CoAkkaHttp 1.0.0 EXACT REQUIRED CONFIG)
find_package(Threads REQUIRED)

option(COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS
       "Treat native test compiler warnings as errors" ON)
option(COAKKA_HTTP_RUNTIME_TEST_ENABLE_SANITIZERS
       "Instrument the native test harness with ASan and UBSan" OFF)
option(COAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN
       "Instrument the native test harness with ThreadSanitizer" OFF)

if(COAKKA_HTTP_RUNTIME_TEST_ENABLE_SANITIZERS AND
   COAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN)
  message(FATAL_ERROR "ASan/UBSan and ThreadSanitizer require separate builds")
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_executable(coakka_http_runtime_c_test native_runtime.c)
set_target_properties(
    coakka_http_runtime_c_test
    PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(
    coakka_http_runtime_c_test PRIVATE CoAkkaHttp::runtime)

add_executable(coakka_http_runtime_cpp_test cpp_header_smoke.cc)
set_target_properties(
    coakka_http_runtime_cpp_test
    PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_link_libraries(
    coakka_http_runtime_cpp_test PRIVATE CoAkkaHttp::runtime)

add_executable(coakka_http_runtime_contract_test native_contract.c)
set_target_properties(
    coakka_http_runtime_contract_test
    PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(
    coakka_http_runtime_contract_test PRIVATE CoAkkaHttp::runtime)

add_executable(
    coakka_http_runtime_concurrency_test
    native_concurrency.c
    test_threads.h
    $<$<PLATFORM_ID:Windows>:test_threads_windows.c>
    $<$<NOT:$<PLATFORM_ID:Windows>>:test_threads_posix.c>)
set_target_properties(
    coakka_http_runtime_concurrency_test
    PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(
    coakka_http_runtime_concurrency_test
    PRIVATE CoAkkaHttp::runtime Threads::Threads)

set(
    COAKKA_HTTP_RUNTIME_TEST_TARGETS
    coakka_http_runtime_c_test
    coakka_http_runtime_cpp_test
    coakka_http_runtime_contract_test
    coakka_http_runtime_concurrency_test)

if(MSVC)
  target_compile_options(
      coakka_http_runtime_c_test
      PRIVATE /W4 $<$<BOOL:${COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS}>:/WX>)
  target_compile_options(
      coakka_http_runtime_cpp_test
      PRIVATE /W4 $<$<BOOL:${COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS}>:/WX>)
  foreach(target IN ITEMS coakka_http_runtime_contract_test
                          coakka_http_runtime_concurrency_test)
    target_compile_options(
        ${target}
        PRIVATE /W4 /experimental:c11atomics
                $<$<BOOL:${COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS}>:/WX>)
  endforeach()
  target_link_libraries(
      coakka_http_runtime_c_test PRIVATE ws2_32)
  target_link_libraries(
      coakka_http_runtime_concurrency_test PRIVATE ws2_32)
else()
  foreach(target IN LISTS COAKKA_HTTP_RUNTIME_TEST_TARGETS)
    target_compile_options(
        ${target}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wsign-conversion
            $<$<BOOL:${COAKKA_HTTP_RUNTIME_TEST_WARNINGS_AS_ERRORS}>:-Werror>)
  endforeach()
  foreach(target IN ITEMS coakka_http_runtime_c_test
                          coakka_http_runtime_contract_test
                          coakka_http_runtime_concurrency_test)
    target_compile_options(${target} PRIVATE -Wstrict-prototypes)
  endforeach()
endif()

if(COAKKA_HTTP_RUNTIME_TEST_ENABLE_SANITIZERS)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR
     NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message(FATAL_ERROR "ASan/UBSan requires Clang or GCC")
  endif()
  foreach(target IN LISTS COAKKA_HTTP_RUNTIME_TEST_TARGETS)
    target_compile_options(
        ${target}
        PRIVATE -fno-omit-frame-pointer -fno-sanitize-recover=all
                -fsanitize=address,undefined)
    target_link_options(${target} PRIVATE -fsanitize=address,undefined)
  endforeach()
elseif(COAKKA_HTTP_RUNTIME_TEST_ENABLE_TSAN)
  if(WIN32 OR NOT CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR
     NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message(FATAL_ERROR
            "ThreadSanitizer requires Clang or GCC on a non-Windows host")
  endif()
  foreach(target IN LISTS COAKKA_HTTP_RUNTIME_TEST_TARGETS)
    target_compile_options(
        ${target}
        PRIVATE -fno-omit-frame-pointer -fno-sanitize-recover=all
                -fsanitize=thread)
    target_link_options(${target} PRIVATE -fsanitize=thread)
  endforeach()
endif()

if(WIN32)
  foreach(target IN LISTS COAKKA_HTTP_RUNTIME_TEST_TARGETS)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:CoAkkaHttp::runtime>
                $<TARGET_FILE_DIR:${target}>
        VERBATIM)
  endforeach()
endif()

add_test(NAME coakka_http_runtime_c_test COMMAND coakka_http_runtime_c_test)
add_test(NAME coakka_http_runtime_cpp_test COMMAND coakka_http_runtime_cpp_test)
add_test(
    NAME coakka_http_runtime_contract_test
    COMMAND coakka_http_runtime_contract_test)
add_test(
    NAME coakka_http_runtime_concurrency_test
    COMMAND coakka_http_runtime_concurrency_test)
set_tests_properties(
    coakka_http_runtime_c_test
    coakka_http_runtime_cpp_test
    coakka_http_runtime_contract_test
    coakka_http_runtime_concurrency_test
    PROPERTIES TIMEOUT 30)
