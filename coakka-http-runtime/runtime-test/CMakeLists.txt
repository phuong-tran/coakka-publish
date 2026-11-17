cmake_minimum_required(VERSION 3.20)
project(CoAkkaHttpRuntimeTest LANGUAGES C CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/verify-source.cmake")
include(CTest)

# Override only when explicitly qualifying a different complete SDK. Header,
# metadata and binary must come from the same admitted package prefix.
set(COAKKA_HTTP_TEST_SDK_VERSION "2.3.0" CACHE STRING "Exact installed SDK version")
find_package(CoAkkaHttpHost ${COAKKA_HTTP_TEST_SDK_VERSION} EXACT REQUIRED CONFIG)
find_package(Threads REQUIRED)
option(COAKKA_HTTP_TEST_FUZZ "Build the optional Clang libFuzzer fixture seam" OFF)
option(COAKKA_HTTP_TEST_RELEASE "Enable the installed release wire suite" OFF)
option(COAKKA_HTTP_TEST_CONTRACT_MATRIX
    "Enable all loopback wire and lifecycle checks for the installed SDK" OFF)
option(COAKKA_HTTP_TEST_HTTP2_WEBSOCKET
    "Exercise standalone HTTP/2 WebSocket using the selected Python with h2" OFF)
option(COAKKA_HTTP_TEST_HTTP3_WEBSOCKET
    "Exercise standalone HTTP/3 WebSocket using the selected Python with aioquic" OFF)
option(COAKKA_HTTP_TEST_HTTP2_ADMISSION_RECLAIM
    "Exercise standalone HTTP/2 admission recovery through installed C and C++ consumers" OFF)
option(COAKKA_HTTP_TEST_HTTP2_ADMISSION_RECLAIM_URING
    "Repeat the HTTP/2 admission recovery cases with effective io_uring" OFF)
option(COAKKA_HTTP_TEST_HTTP3_CONNECTION_CAPACITY
    "Verify the effective standalone HTTP/3 connection ceiling through installed C and C++ consumers" OFF)
option(COAKKA_HTTP_TEST_HTTP3_CONNECTION_CAPACITY_URING
    "Repeat the standalone HTTP/3 connection ceiling case with effective io_uring" OFF)
option(COAKKA_HTTP_TEST_HTTP3_UPLOAD_PRESSURE
    "Require the HTTP/3 default and HTTP/2 and HTTP/3 held-reader upload checks" OFF)
option(COAKKA_HTTP_TEST_HTTP1_UPLOAD_DRAIN
    "Exercise HTTP/1.1 upload drain and shutdown using plaintext and TLS" OFF)
option(COAKKA_HTTP_TEST_HTTP1_EPHEMERAL_RELISTEN
    "Exercise HTTP/1.1 restart on the exact port returned by an installed SDK" OFF)
option(COAKKA_HTTP_TEST_HTTP2_UPLOAD_DRAIN
    "Exercise held HTTP/2 upload leases with the selected Python and h2" OFF)
option(COAKKA_HTTP_TEST_HTTP3_UPLOAD_DRAIN
    "Exercise held HTTP/3 upload leases with the selected Python and aioquic" OFF)
option(COAKKA_HTTP_TEST_LIFECYCLE_INTERACTIONS
    "Exercise standalone, multiplexed upload reset, file drain and write deadlines" OFF)
option(COAKKA_HTTP_TEST_HTTP2_UPLOAD_RECONNECT_URING
    "Repeat the HTTP/2 upload reconnection case with effective io_uring" OFF)
option(COAKKA_HTTP_TEST_OWNER_LIVENESS
    "Exercise idle, drain and stop liveness through the standalone installed SDK" OFF)
option(COAKKA_HTTP_TEST_OWNER_LIVENESS_URING
    "Repeat standalone HTTP/2 and HTTP/3 liveness with effective io_uring" OFF)
option(COAKKA_HTTP_TEST_RESPONSE_POLICY
    "Exercise slow SSE isolation and gzip fallback through installed response APIs" OFF)
option(COAKKA_HTTP_TEST_NATIVE_FINAL_GATES
    "Exercise installed monitor, static file recovery and outbound transport" OFF)
option(COAKKA_HTTP_TEST_HTTP3_FILE_DRAIN
    "Exercise file drain with and without a final HTTP/3 acknowledgement" OFF)
set(COAKKA_HTTP_TEST_CURL "" CACHE FILEPATH
    "Enable endpoint protocol tests with this HTTP/2 and HTTP/3 capable curl")
set(COAKKA_HTTP_TEST_CERTIFICATE "" CACHE FILEPATH "Loopback certificate for release wire checks")
set(COAKKA_HTTP_TEST_PRIVATE_KEY "" CACHE FILEPATH "Loopback private key for release wire checks")
set(COAKKA_HTTP_TEST_CA_CERTIFICATE "" CACHE FILEPATH "Trust root for loopback wire checks")
set(COAKKA_HTTP_TEST_OUTBOUND_LAN_ADDRESS "" CACHE STRING
    "Non-loopback IPv4 address already assigned to the test host")

if(COAKKA_HTTP_TEST_CONTRACT_MATRIX)
  if(NOT BUILD_TESTING OR COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.3.0")
    message(FATAL_ERROR "The contract matrix requires BUILD_TESTING and SDK 2.3.0 or later")
  endif()
  foreach(group IN ITEMS RELEASE HTTP2_WEBSOCKET HTTP3_WEBSOCKET
      HTTP2_ADMISSION_RECLAIM HTTP3_CONNECTION_CAPACITY HTTP3_UPLOAD_PRESSURE
      HTTP1_UPLOAD_DRAIN HTTP1_EPHEMERAL_RELISTEN HTTP2_UPLOAD_DRAIN HTTP3_UPLOAD_DRAIN
      LIFECYCLE_INTERACTIONS NATIVE_FINAL_GATES HTTP3_FILE_DRAIN RESPONSE_POLICY OWNER_LIVENESS)
    set(COAKKA_HTTP_TEST_${group} ON)
  endforeach()
endif()

add_library(http_test_options INTERFACE)
target_compile_features(http_test_options INTERFACE c_std_11 cxx_std_20)
if(MSVC)
  target_compile_options(http_test_options INTERFACE /W4 /WX)
else()
  target_compile_options(http_test_options INTERFACE
    -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Werror
    $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)
endif()

add_library(http_test_support STATIC test_support.c test_socket.c test_wire.c)
set_target_properties(http_test_support PROPERTIES C_EXTENSIONS NO)
target_link_libraries(http_test_support PUBLIC http_test_options CoAkkaHttp::host)
if(WIN32)
  target_link_libraries(http_test_support PUBLIC ws2_32)
endif()
add_library(http_test_threads STATIC
  $<$<PLATFORM_ID:Windows>:test_threads_windows.c>
  $<$<NOT:$<PLATFORM_ID:Windows>>:test_threads_posix.c>)
set_target_properties(http_test_threads PROPERTIES C_EXTENSIONS NO)
target_link_libraries(http_test_threads PUBLIC http_test_options Threads::Threads)

function(configure_test_target name)
  set_target_properties(${name} PROPERTIES C_EXTENSIONS NO CXX_EXTENSIONS NO)
  if(WIN32)
    add_custom_command(TARGET ${name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:CoAkkaHttp::host> $<TARGET_FILE_DIR:${name}> VERBATIM)
  endif()
endfunction()

function(register_test name)
  configure_test_target(${name})
  add_test(NAME ${name} COMMAND ${name} ${ARGN})
  # One process-level watchdog complements finite Core and socket waits. The
  # thread helper never frees a child context when completion is unproven.
  set_tests_properties(${name} PROPERTIES TIMEOUT 120)
endfunction()

foreach(case IN ITEMS runtime contract ownership pressure outbound routes stream concurrency execution)
  add_executable(http_host_${case} host_${case}.c)
  target_link_libraries(http_host_${case} PRIVATE http_test_support)
  register_test(http_host_${case})
endforeach()
add_executable(http_host_upload_cancel host_upload_cancel.c)
target_link_libraries(http_host_upload_cancel PRIVATE http_test_support)
register_test(http_host_upload_cancel)
add_executable(http_host_outbound_address host_outbound_address.c)
target_link_libraries(http_host_outbound_address PRIVATE http_test_support)
register_test(http_host_outbound_address localhost 9 /address rejected)
if(NOT COAKKA_HTTP_TEST_OUTBOUND_LAN_ADDRESS STREQUAL "")
  if(NOT COAKKA_HTTP_TEST_OUTBOUND_LAN_ADDRESS MATCHES
      "^[0-9]+[.][0-9]+[.][0-9]+[.][0-9]+$" OR
      COAKKA_HTTP_TEST_OUTBOUND_LAN_ADDRESS MATCHES "^(0[.]|127[.])")
    message(FATAL_ERROR "The outbound LAN case requires a local non-loopback IPv4 address")
  endif()
  add_test(NAME http_host_outbound_lan
    COMMAND http_host_outbound "${COAKKA_HTTP_TEST_OUTBOUND_LAN_ADDRESS}")
  set_tests_properties(http_host_outbound_lan PROPERTIES TIMEOUT 120)
endif()
target_link_libraries(http_host_concurrency PRIVATE http_test_threads)
# Version 1.1.2 introduces the focused TLS credential control surface.
if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_GREATER_EQUAL "1.1.2")
  add_executable(http_host_tls_reload host_tls_reload.c)
  target_link_libraries(http_host_tls_reload PRIVATE http_test_support)
  register_test(http_host_tls_reload)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tls-fixture")
  set_tests_properties(http_host_tls_reload PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tls-fixture")
endif()
if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_GREATER_EQUAL "2.1.0")
  add_executable(http_host_multi_listener host_multi_listener.c)
  target_link_libraries(http_host_multi_listener PRIVATE http_test_support)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_GREATER_EQUAL "2.3.0")
    target_compile_definitions(http_host_multi_listener PRIVATE
      COAKKA_HTTP_TEST_WEBSOCKET_PROFILES=1)
  endif()
  register_test(http_host_multi_listener)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/v1-fixture")
  set_tests_properties(http_host_multi_listener PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/v1-fixture")
endif()
if(COAKKA_HTTP_TEST_HTTP2_ADMISSION_RECLAIM)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import h2"
    RESULT_VARIABLE h2_result TIMEOUT 5)
  if(NOT h2_result STREQUAL "0")
    message(FATAL_ERROR "HTTP/2 admission recovery fixtures require the Python h2 package")
  endif()
  if(NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
    message(FATAL_ERROR "HTTP/2 admission recovery needs the loopback certificate, private key and trust root")
  endif()
  set(h2_reclaim_cpp "${CMAKE_CURRENT_BINARY_DIR}/host_h2_admission_reclaim.cc")
  configure_file(host_h2_admission_reclaim.c "${h2_reclaim_cpp}" COPYONLY)
  add_executable(http_host_h2_admission_reclaim_c host_h2_admission_reclaim.c)
  add_executable(http_host_h2_admission_reclaim_cpp "${h2_reclaim_cpp}")
  set(h2_reclaim_backends default)
  if(COAKKA_HTTP_TEST_HTTP2_ADMISSION_RECLAIM_URING)
    list(APPEND h2_reclaim_backends uring)
  endif()
  foreach(language IN ITEMS c cpp)
    set(target "http_host_h2_admission_reclaim_${language}")
    target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("${target}" PRIVATE http_test_support)
    configure_test_target("${target}")
    foreach(backend IN LISTS h2_reclaim_backends)
      foreach(transport IN ITEMS tls plain)
        add_test(NAME "${target}_${backend}_${transport}"
          COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/h2_admission_reclaim_wire.py" "$<TARGET_FILE:${target}>"
            "${COAKKA_HTTP_TEST_CERTIFICATE}" "${COAKKA_HTTP_TEST_PRIVATE_KEY}"
            "${COAKKA_HTTP_TEST_CA_CERTIFICATE}" --backend "${backend}" --transport "${transport}")
        set_tests_properties("${target}_${backend}_${transport}" PROPERTIES TIMEOUT 90)
      endforeach()
    endforeach()
  endforeach()
endif()
if(COAKKA_HTTP_TEST_HTTP1_EPHEMERAL_RELISTEN)
  find_package(Python3 3.11 REQUIRED COMPONENTS Interpreter)
  set(h1_relisten_cpp "${CMAKE_CURRENT_BINARY_DIR}/host_h1_ephemeral_relisten.cc")
  configure_file(host_h1_ephemeral_relisten.c "${h1_relisten_cpp}" COPYONLY)
  add_executable(http_host_h1_ephemeral_relisten_c host_h1_ephemeral_relisten.c)
  add_executable(http_host_h1_ephemeral_relisten_cpp "${h1_relisten_cpp}")
  foreach(language IN ITEMS c cpp)
    set(target "http_host_h1_ephemeral_relisten_${language}")
    target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("${target}" PRIVATE http_test_support)
    configure_test_target("${target}")
    foreach(family IN ITEMS ipv4 ipv6)
      if(family STREQUAL "ipv4")
        set(address "127.0.0.1")
      else()
        set(address "::1")
      endif()
      foreach(security IN ITEMS plain tls mtls)
        foreach(scenario IN ITEMS ephemeral_request fixed_request ephemeral_idle)
          string(REPLACE "_" ";" inputs "${scenario}")
          set(name "${target}_${family}_${security}_${scenario}")
          add_test(NAME "${name}" COMMAND "${Python3_EXECUTABLE}" -B
            "${CMAKE_CURRENT_SOURCE_DIR}/h1_ephemeral_relisten_wire.py"
            "$<TARGET_FILE:${target}>" "${address}" "${security}" ${inputs})
          set_tests_properties("${name}" PROPERTIES TIMEOUT 60)
        endforeach()
      endforeach()
    endforeach()
  endforeach()
endif()
if(COAKKA_HTTP_TEST_HTTP3_CONNECTION_CAPACITY)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import aioquic"
    RESULT_VARIABLE aioquic_result TIMEOUT 5)
  if(NOT aioquic_result STREQUAL "0" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
    message(FATAL_ERROR "HTTP/3 connection capacity needs aioquic and loopback TLS material")
  endif()
  set(capacity_cpp "${CMAKE_CURRENT_BINARY_DIR}/host_h3_connection_capacity.cc")
  configure_file(host_h3_connection_capacity.c "${capacity_cpp}" COPYONLY)
  add_executable(http_host_h3_connection_capacity_c host_h3_connection_capacity.c)
  add_executable(http_host_h3_connection_capacity_cpp "${capacity_cpp}")
  set(capacity_backends default)
  if(COAKKA_HTTP_TEST_HTTP3_CONNECTION_CAPACITY_URING)
    list(APPEND capacity_backends uring)
  endif()
  foreach(language IN ITEMS c cpp)
    set(target "http_host_h3_connection_capacity_${language}")
    target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("${target}" PRIVATE http_test_support)
    configure_test_target("${target}")
    foreach(backend IN LISTS capacity_backends)
      add_test(NAME "${target}_${backend}"
        COMMAND "${Python3_EXECUTABLE}" -B
          "${CMAKE_CURRENT_SOURCE_DIR}/h3_connection_capacity_wire.py" "$<TARGET_FILE:${target}>"
          "${COAKKA_HTTP_TEST_CERTIFICATE}" "${COAKKA_HTTP_TEST_PRIVATE_KEY}"
          "${COAKKA_HTTP_TEST_CA_CERTIFICATE}" --backend "${backend}")
      set_tests_properties("${target}_${backend}" PROPERTIES TIMEOUT 90)
    endforeach()
  endforeach()
endif()
if(COAKKA_HTTP_TEST_HTTP2_WEBSOCKET)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Standalone HTTP/2 WebSocket requires a Core 2.3 or later SDK with the feature")
  endif()
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import h2"
    RESULT_VARIABLE h2_result TIMEOUT 5)
  if(NOT h2_result STREQUAL "0")
    message(FATAL_ERROR "HTTP/2 WebSocket fixtures require the Python h2 package")
  endif()
  add_executable(http_host_http2_websocket host_http2_websocket.c)
  target_link_libraries(http_host_http2_websocket PRIVATE http_test_support)
  configure_test_target(http_host_http2_websocket)
  foreach(mode IN ITEMS off multiplex graceful expired shutdown)
    add_test(NAME http_host_http2_websocket_${mode}
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/http2_websocket_wire.py"
        $<TARGET_FILE:http_host_http2_websocket> ${mode})
    set_tests_properties(http_host_http2_websocket_${mode} PROPERTIES TIMEOUT 30)
  endforeach()
endif()
if(COAKKA_HTTP_TEST_HTTP3_WEBSOCKET)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Standalone HTTP/3 WebSocket requires SDK 2.3.0 or later")
  endif()
  find_package(Python3 3.11 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -B -c "import aioquic"
    RESULT_VARIABLE aioquic_result TIMEOUT 5)
  if(NOT aioquic_result STREQUAL "0"
      OR NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}"
      OR NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}"
      OR NOT EXISTS "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
    message(FATAL_ERROR "HTTP/3 WebSocket requires aioquic and loopback TLS certificate, key and CA files")
  endif()
  add_executable(http_host_http3_websocket host_http3_websocket.c)
  target_link_libraries(http_host_http3_websocket PRIVATE http_test_support http_test_threads)
  configure_test_target(http_host_http3_websocket)
  foreach(mode IN ITEMS ordinary ordinary-with-ws reset-stream reset-stop reset-fin
      reset-pre-accept reset-pre-accept-stop reset-pre-accept-fin
      reset-pending-write reset-pending-write-stream reset-pending-write-fin
      complete expired aborted shutdown)
    add_test(NAME http_host_http3_websocket_${mode}
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/http3_websocket_peer.py"
        --ca "${COAKKA_HTTP_TEST_CA_CERTIFICATE}" $<TARGET_FILE:http_host_http3_websocket>
        "${COAKKA_HTTP_TEST_CERTIFICATE}" "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${mode}")
    set_tests_properties(http_host_http3_websocket_${mode} PROPERTIES TIMEOUT 35)
  endforeach()
endif()
if(COAKKA_HTTP_TEST_RELEASE AND NOT COAKKA_HTTP_TEST_CURL)
  message(FATAL_ERROR "Release tests require COAKKA_HTTP_TEST_CURL")
endif()
if(COAKKA_HTTP_TEST_HTTP3_FILE_DRAIN AND NOT COAKKA_HTTP_TEST_RELEASE)
  message(FATAL_ERROR "HTTP/3 file drain requires COAKKA_HTTP_TEST_RELEASE=ON")
endif()
if(COAKKA_HTTP_TEST_HTTP3_UPLOAD_PRESSURE AND NOT COAKKA_HTTP_TEST_RELEASE)
  message(FATAL_ERROR "HTTP/3 upload pressure requires COAKKA_HTTP_TEST_RELEASE")
endif()
if(COAKKA_HTTP_TEST_NATIVE_FINAL_GATES)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Native final gates require SDK 2.3.0 or newer")
  endif()
  find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
  if(NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
    message(FATAL_ERROR "Native final gates require loopback TLS material")
  endif()
  foreach(case IN ITEMS file_gate monitor_gate outbound_gate)
    add_executable(http_host_${case} host_${case}.c)
    target_link_libraries(http_host_${case} PRIVATE http_test_support)
    configure_test_target(http_host_${case})
  endforeach()
  target_link_libraries(http_host_file_gate PRIVATE Threads::Threads)
  add_executable(http_host_outbound_start_stack host_outbound_start_stack.c)
  target_link_libraries(http_host_outbound_start_stack PRIVATE http_test_support Threads::Threads)
  configure_test_target(http_host_outbound_start_stack)
  foreach(mode IN ITEMS plain outbound-large-stack outbound)
    add_test(NAME "http_host_outbound_start_stack_${mode}"
      COMMAND http_host_outbound_start_stack "${mode}")
    set_tests_properties("http_host_outbound_start_stack_${mode}" PROPERTIES TIMEOUT 15)
  endforeach()
  add_test(NAME http_host_monitor_gate COMMAND http_host_monitor_gate)
  add_test(NAME http_host_outbound_gate
    COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/outbound_gate_wire.py"
      $<TARGET_FILE:http_host_outbound_gate> "${COAKKA_HTTP_TEST_CERTIFICATE}"
      "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
  foreach(mode IN ITEMS short large-head large-body large-declared-body body-boundary)
    add_test(NAME "http_host_outbound_gate_${mode}"
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/outbound_gate_wire.py"
        $<TARGET_FILE:http_host_outbound_gate> "${COAKKA_HTTP_TEST_CERTIFICATE}"
        "${COAKKA_HTTP_TEST_PRIVATE_KEY}" --response-case "${mode}")
    set_tests_properties("http_host_outbound_gate_${mode}" PROPERTIES TIMEOUT 40)
  endforeach()
  foreach(mode IN ITEMS plain tls)
    add_test(NAME "http_host_file_gate_${mode}"
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/file_gate_wire.py"
        $<TARGET_FILE:http_host_file_gate> "${COAKKA_HTTP_TEST_CERTIFICATE}"
        "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${CMAKE_CURRENT_BINARY_DIR}" "${mode}")
    set_tests_properties("http_host_file_gate_${mode}" PROPERTIES TIMEOUT 40)
  endforeach()
  set_tests_properties(http_host_monitor_gate http_host_outbound_gate PROPERTIES TIMEOUT 40)
endif()
if(COAKKA_HTTP_TEST_LIFECYCLE_INTERACTIONS)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Lifecycle interactions require SDK 2.3.0 or newer")
  endif()
  find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import h2, aioquic"
    RESULT_VARIABLE lifecycle_peer_result TIMEOUT 5)
  if(NOT lifecycle_peer_result STREQUAL "0" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
    message(FATAL_ERROR "Lifecycle interactions require h2, aioquic and loopback TLS material")
  endif()
  set(h2_upload_reconnect_cpp "${CMAKE_CURRENT_BINARY_DIR}/host_h2_upload_reconnect.cc")
  configure_file(host_h2_upload_reconnect.c "${h2_upload_reconnect_cpp}" COPYONLY)
  add_executable(http_host_h2_upload_reconnect_c host_h2_upload_reconnect.c)
  add_executable(http_host_h2_upload_reconnect_cpp "${h2_upload_reconnect_cpp}")
  set(h2_upload_reconnect_backends default)
  if(COAKKA_HTTP_TEST_HTTP2_UPLOAD_RECONNECT_URING)
    list(APPEND h2_upload_reconnect_backends uring)
  endif()
  set(h2_upload_reconnect_ca "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
  if(NOT h2_upload_reconnect_ca)
    set(h2_upload_reconnect_ca "${COAKKA_HTTP_TEST_CERTIFICATE}")
  endif()
  foreach(language IN ITEMS c cpp)
    set(target "http_host_h2_upload_reconnect_${language}")
    target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("${target}" PRIVATE http_test_support)
    configure_test_target("${target}")
    foreach(backend IN LISTS h2_upload_reconnect_backends)
      foreach(mode IN ITEMS poll timed)
        set(case "${target}_${backend}_${mode}")
        add_test(NAME "${case}"
          COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/h2_upload_reconnect_wire.py"
            $<TARGET_FILE:${target}> "${COAKKA_HTTP_TEST_CERTIFICATE}"
            "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${h2_upload_reconnect_ca}"
            --backend "${backend}" --mode "${mode}")
        set_tests_properties("${case}" PROPERTIES TIMEOUT 30)
      endforeach()
    endforeach()
  endforeach()
  add_executable(http_host_lifecycle_interactions host_lifecycle_interactions.c)
  target_link_libraries(http_host_lifecycle_interactions PRIVATE http_test_support)
  configure_test_target(http_host_lifecycle_interactions)
  set(lifecycle_trust_args)
  if(COAKKA_HTTP_TEST_CA_CERTIFICATE)
    list(APPEND lifecycle_trust_args --ca "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
  endif()
  foreach(protocol IN ITEMS 2 3)
    foreach(mode IN ITEMS reset-before-data reset-held-data file-drain write-timeout multipart-reset)
      set(case "http_host_lifecycle_h${protocol}_${mode}")
      add_test(NAME "${case}"
        COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/lifecycle_interactions_wire.py"
          $<TARGET_FILE:http_host_lifecycle_interactions> "${COAKKA_HTTP_TEST_CERTIFICATE}"
          "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${CMAKE_CURRENT_BINARY_DIR}"
          "${mode}" --protocol "${protocol}" ${lifecycle_trust_args})
      set_tests_properties("${case}" PROPERTIES TIMEOUT 35)
    endforeach()
  endforeach()
  foreach(mode IN ITEMS reset-data-followup reset-data-followup-shared)
    set(case "http_host_lifecycle_h3_${mode}")
    add_test(NAME "${case}"
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/lifecycle_interactions_wire.py"
        $<TARGET_FILE:http_host_lifecycle_interactions> "${COAKKA_HTTP_TEST_CERTIFICATE}"
        "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${CMAKE_CURRENT_BINARY_DIR}"
        "${mode}" --protocol 3 ${lifecycle_trust_args})
    set_tests_properties("${case}" PROPERTIES TIMEOUT 40)
  endforeach()
endif()
if(COAKKA_HTTP_TEST_LIFECYCLE_INTERACTIONS)
  add_test(NAME http_host_lifecycle_h1_multipart-reset
    COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/lifecycle_interactions_wire.py"
      $<TARGET_FILE:http_host_lifecycle_interactions> "${COAKKA_HTTP_TEST_CERTIFICATE}"
      "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${CMAKE_CURRENT_BINARY_DIR}"
      multipart-reset --protocol 1.1)
  set_tests_properties(http_host_lifecycle_h1_multipart-reset PROPERTIES TIMEOUT 35)
endif()
if(COAKKA_HTTP_TEST_HTTP1_UPLOAD_DRAIN OR (COAKKA_HTTP_TEST_RELEASE AND
                                            COAKKA_HTTP_TEST_SDK_VERSION VERSION_GREATER_EQUAL "2.3.0"))
  find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
  if(NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
    message(FATAL_ERROR "HTTP/1.1 upload drain requires loopback TLS material")
  endif()
  add_executable(http_host_upload_drain_h1 host_upload_drain_h1.c)
  target_link_libraries(http_host_upload_drain_h1 PRIVATE http_test_support)
  configure_test_target(http_host_upload_drain_h1)
  add_test(NAME http_host_upload_drain_h1
    COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/upload_drain_h1_wire.py"
      $<TARGET_FILE:http_host_upload_drain_h1> "${COAKKA_HTTP_TEST_CERTIFICATE}"
      "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
  set_tests_properties(http_host_upload_drain_h1 PROPERTIES TIMEOUT 90)
endif()
if(COAKKA_HTTP_TEST_HTTP2_UPLOAD_DRAIN OR COAKKA_HTTP_TEST_HTTP3_UPLOAD_DRAIN)
  find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
  if(NOT EXISTS "${COAKKA_HTTP_TEST_CERTIFICATE}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
    message(FATAL_ERROR "Upload drain requires loopback TLS material")
  endif()
  add_executable(http_host_upload_drain host_upload_drain.c)
  target_link_libraries(http_host_upload_drain PRIVATE http_test_support)
  configure_test_target(http_host_upload_drain)
endif()
if(COAKKA_HTTP_TEST_HTTP2_UPLOAD_DRAIN)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import h2"
    RESULT_VARIABLE h2_result TIMEOUT 5)
  if(NOT h2_result STREQUAL "0")
    message(FATAL_ERROR "HTTP/2 upload drain requires Python h2")
  endif()
  add_test(NAME http_host_upload_drain_h2
    COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/upload_drain_h2_wire.py"
      $<TARGET_FILE:http_host_upload_drain> "${COAKKA_HTTP_TEST_CERTIFICATE}"
      "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
  set_tests_properties(http_host_upload_drain_h2 PROPERTIES TIMEOUT 60)
endif()
if(COAKKA_HTTP_TEST_HTTP3_UPLOAD_DRAIN)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import aioquic"
    RESULT_VARIABLE aioquic_result TIMEOUT 5)
  if(NOT aioquic_result STREQUAL "0")
    message(FATAL_ERROR "HTTP/3 upload drain requires Python aioquic")
  endif()
  add_test(NAME http_host_upload_drain
    COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/upload_drain_wire.py"
      $<TARGET_FILE:http_host_upload_drain> "${COAKKA_HTTP_TEST_CERTIFICATE}"
      "${COAKKA_HTTP_TEST_PRIVATE_KEY}")
  set_tests_properties(http_host_upload_drain PROPERTIES TIMEOUT 60)
endif()
if(COAKKA_HTTP_TEST_OWNER_LIVENESS)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Liveness tests require SDK 2.3.0 or newer")
  endif()
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import h2, aioquic"
    RESULT_VARIABLE liveness_python_result TIMEOUT 5)
  if(NOT liveness_python_result STREQUAL "0")
    message(FATAL_ERROR "Liveness fixtures require Python h2 and aioquic")
  endif()
  foreach(input IN ITEMS CERTIFICATE PRIVATE_KEY CA_CERTIFICATE)
    if(NOT IS_ABSOLUTE "${COAKKA_HTTP_TEST_${input}}" OR
       NOT EXISTS "${COAKKA_HTTP_TEST_${input}}")
      message(FATAL_ERROR "Liveness fixtures require COAKKA_HTTP_TEST_${input}")
    endif()
  endforeach()

  add_executable(http_host_liveness_lifecycle host_liveness_lifecycle.c)
  target_link_libraries(http_host_liveness_lifecycle PRIVATE http_test_support http_test_threads)
  configure_test_target(http_host_liveness_lifecycle)

  function(register_liveness profile backend monitor)
    set(case "http_host_liveness_${profile}_${backend}_${monitor}")
    add_test(NAME "${case}" COMMAND "${Python3_EXECUTABLE}" -B
      "${CMAKE_CURRENT_SOURCE_DIR}/liveness_wire.py"
      $<TARGET_FILE:http_host_liveness_lifecycle>
      "${COAKKA_HTTP_TEST_CERTIFICATE}" "${COAKKA_HTTP_TEST_PRIVATE_KEY}"
      "${COAKKA_HTTP_TEST_CA_CERTIFICATE}" "${profile}" "${backend}" "${monitor}")
    set_tests_properties("${case}" PROPERTIES TIMEOUT 70)
  endfunction()

  foreach(profile IN ITEMS h1 h1-tls h2 h3)
    foreach(monitor IN ITEMS disabled monitor)
      register_liveness("${profile}" default "${monitor}")
    endforeach()
  endforeach()
  if(COAKKA_HTTP_TEST_OWNER_LIVENESS_URING)
    foreach(profile IN ITEMS h2 h3)
      foreach(monitor IN ITEMS disabled monitor)
        register_liveness("${profile}" uring "${monitor}")
      endforeach()
    endforeach()
  endif()
endif()
if(COAKKA_HTTP_TEST_RESPONSE_POLICY)
  find_package(Python3 3.11 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import h2, aioquic"
    RESULT_VARIABLE policy_peers TIMEOUT 5)
  if(NOT policy_peers STREQUAL "0")
    message(FATAL_ERROR "Response policy fixtures require Python h2 and aioquic")
  endif()
  foreach(input IN ITEMS CERTIFICATE PRIVATE_KEY CA_CERTIFICATE)
    if(NOT IS_ABSOLUTE "${COAKKA_HTTP_TEST_${input}}" OR
       NOT EXISTS "${COAKKA_HTTP_TEST_${input}}")
      message(FATAL_ERROR "Response policy fixtures require COAKKA_HTTP_TEST_${input}")
    endif()
  endforeach()
  add_executable(http_host_response_policy host_response_policy.c)
  target_link_libraries(http_host_response_policy PRIVATE http_test_support)
  configure_test_target(http_host_response_policy)
  foreach(profile IN ITEMS 1.1 2 3)
    set(modes sse-pressure gzip)
    if(profile STREQUAL "1.1")
      list(APPEND modes write-timeout)
    endif()
    foreach(mode IN LISTS modes)
      set(case "http_host_response_policy_${mode}_http${profile}")
      add_test(NAME "${case}" COMMAND "${Python3_EXECUTABLE}" -B
        "${CMAKE_CURRENT_SOURCE_DIR}/response_policy_wire.py"
        $<TARGET_FILE:http_host_response_policy> "${mode}" "${profile}"
        "${COAKKA_HTTP_TEST_CERTIFICATE}" "${COAKKA_HTTP_TEST_PRIVATE_KEY}"
        "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
      set_tests_properties("${case}" PROPERTIES TIMEOUT 45)
    endforeach()
  endforeach()
endif()
if(COAKKA_HTTP_TEST_CURL)
  if(COAKKA_HTTP_TEST_SDK_VERSION VERSION_LESS "2.1.0")
    message(FATAL_ERROR "The endpoint protocol tests require SDK 2.1.0 or later")
  endif()
  if(NOT IS_ABSOLUTE "${COAKKA_HTTP_TEST_CURL}" OR
     NOT EXISTS "${COAKKA_HTTP_TEST_CURL}" OR IS_DIRECTORY "${COAKKA_HTTP_TEST_CURL}")
    message(FATAL_ERROR "COAKKA_HTTP_TEST_CURL must name an absolute curl executable")
  endif()
  find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter)
  if(COAKKA_HTTP_TEST_RELEASE AND COAKKA_HTTP_TEST_SDK_VERSION VERSION_GREATER_EQUAL "2.3.0")
    add_executable(http_host_tls_reload_wire host_tls_reload_wire.c)
    target_link_libraries(http_host_tls_reload_wire PRIVATE http_test_support)
    configure_test_target(http_host_tls_reload_wire)
    foreach(protocol IN ITEMS 1.1 2 3)
      foreach(security IN ITEMS tls mtls)
        set(case "http_host_tls_reload_wire_${protocol}_${security}")
        add_test(NAME "${case}"
          COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/tls_reload_wire.py"
            $<TARGET_FILE:http_host_tls_reload_wire> "${COAKKA_HTTP_TEST_CURL}"
            "${protocol}" "${security}")
        set_tests_properties("${case}" PROPERTIES TIMEOUT 45)
      endforeach()
    endforeach()
  endif()
  add_executable(http_host_protocols host_protocols.c)
  target_link_libraries(http_host_protocols PRIVATE http_test_support)
  configure_test_target(http_host_protocols)
  foreach(profile IN ITEMS 1.1 2 3 mixed)
    set(case "http_host_protocol_${profile}")
    add_test(NAME "${case}"
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/check_protocols.py"
        $<TARGET_FILE:http_host_protocols> "${COAKKA_HTTP_TEST_CURL}" "${profile}")
    set_tests_properties("${case}" PROPERTIES TIMEOUT 60)
    if(COAKKA_HTTP_TEST_CONTRACT_MATRIX)
      set(capacity_case "http_host_response_capacity_${profile}")
      add_test(NAME "${capacity_case}"
        COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/check_protocols.py"
          $<TARGET_FILE:http_host_protocols> "${COAKKA_HTTP_TEST_CURL}" "${profile}" --capacity)
      set_tests_properties("${capacity_case}" PROPERTIES TIMEOUT 75)
    endif()
  endforeach()
endif()
if(COAKKA_HTTP_TEST_RELEASE)
  find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
  foreach(input IN ITEMS CERTIFICATE PRIVATE_KEY)
    set(path "${COAKKA_HTTP_TEST_${input}}")
    if(NOT IS_ABSOLUTE "${path}" OR NOT EXISTS "${path}" OR IS_DIRECTORY "${path}")
      message(FATAL_ERROR "COAKKA_HTTP_TEST_${input} must name an absolute fixture file")
    endif()
  endforeach()
  execute_process(COMMAND "${COAKKA_HTTP_TEST_CURL}" -V
    RESULT_VARIABLE curl_result OUTPUT_VARIABLE curl_version TIMEOUT 5)
  if(NOT curl_result STREQUAL "0"
      OR NOT curl_version MATCHES "Features:[^\n]*[ \t]HTTP2([ \t\r\n]|$)"
      OR NOT curl_version MATCHES "Features:[^\n]*[ \t]HTTP3([ \t\r\n]|$)")
    message(FATAL_ERROR "Release tests require curl with HTTP/2 and HTTP/3")
  endif()
  add_executable(http_host_stream_standalone host_stream_standalone.c)
  target_link_libraries(http_host_stream_standalone PRIVATE http_test_options CoAkkaHttp::host)
  configure_test_target(http_host_stream_standalone)
  foreach(case IN ITEMS stream sse)
    foreach(version IN ITEMS 2 3)
      add_test(NAME http_host_${case}_http${version}
        COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/stream_wire.py"
          "${case}" "${version}" "${COAKKA_HTTP_TEST_CURL}" "${COAKKA_HTTP_TEST_CERTIFICATE}"
          "${COAKKA_HTTP_TEST_PRIVATE_KEY}" $<TARGET_FILE:http_host_stream_standalone>)
      set_tests_properties(http_host_${case}_http${version} PROPERTIES TIMEOUT 30)
    endforeach()
  endforeach()
  add_executable(http_host_upload_standalone host_upload_standalone.c)
  target_link_libraries(http_host_upload_standalone PRIVATE http_test_support)
  configure_test_target(http_host_upload_standalone)
  function(register_wire_upload name version bytes retained_items)
    add_test(NAME "${name}"
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/stream_wire.py"
        upload "${version}" "${COAKKA_HTTP_TEST_CURL}" "${COAKKA_HTTP_TEST_CERTIFICATE}"
        "${COAKKA_HTTP_TEST_PRIVATE_KEY}" $<TARGET_FILE:http_host_upload_standalone>
        "${bytes}" "${retained_items}" ${ARGN})
    set_tests_properties("${name}" PROPERTIES TIMEOUT 75)
  endfunction()
  register_wire_upload(http_host_upload_http2 2 16384 0)
  register_wire_upload(http_host_upload_http3 3 512 0)
  if(COAKKA_HTTP_TEST_HTTP3_UPLOAD_PRESSURE)
    register_wire_upload(http_host_upload_http2_held_reader_64k 2 65536 1 hold-first-data)
    register_wire_upload(http_host_upload_http3_default_16k 3 16384 0)
    register_wire_upload(http_host_upload_http3_held_reader_16k 3 16384 1 hold-first-data)
    register_wire_upload(http_host_upload_http3_provider_64k 3 65536 128)
    foreach(version IN ITEMS 2 3)
      register_wire_upload(http_host_upload_http${version}_cancel_held
        ${version} 65536 1 cancel-held-data)
    endforeach()
  endif()
  add_executable(http_host_files_standalone host_files_standalone.c)
  target_link_libraries(http_host_files_standalone PRIVATE http_test_support)
  configure_test_target(http_host_files_standalone)
  foreach(version IN ITEMS 1.1 2 3)
    add_test(NAME http_host_files_http${version}
      COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/files_wire.py"
        "${version}" "${COAKKA_HTTP_TEST_CURL}" "${COAKKA_HTTP_TEST_CERTIFICATE}"
        "${COAKKA_HTTP_TEST_PRIVATE_KEY}" "${CMAKE_CURRENT_BINARY_DIR}"
        $<TARGET_FILE:http_host_files_standalone>)
    set_tests_properties(http_host_files_http${version} PROPERTIES TIMEOUT 75)
  endforeach()
  if(COAKKA_HTTP_TEST_HTTP3_FILE_DRAIN)
    execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import aioquic"
      RESULT_VARIABLE aioquic_result TIMEOUT 5)
    if(NOT aioquic_result STREQUAL "0")
      message(FATAL_ERROR "HTTP/3 file drain requires the Python aioquic package")
    endif()
    set(file_drain_ca "${COAKKA_HTTP_TEST_CA_CERTIFICATE}")
    if(file_drain_ca STREQUAL "")
      set(file_drain_ca "${COAKKA_HTTP_TEST_CERTIFICATE}")
    endif()
    if(NOT EXISTS "${file_drain_ca}" OR IS_DIRECTORY "${file_drain_ca}")
      message(FATAL_ERROR "HTTP/3 file drain requires a loopback trust file")
    endif()
    foreach(mode IN ITEMS ack expire sibling)
      add_test(NAME http_host_files_http3_drain_${mode}
        COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/files_drain_wire.py"
          "${mode}" "${COAKKA_HTTP_TEST_CERTIFICATE}" "${COAKKA_HTTP_TEST_PRIVATE_KEY}"
          "${CMAKE_CURRENT_BINARY_DIR}" $<TARGET_FILE:http_host_files_standalone> "${file_drain_ca}")
      set_tests_properties(http_host_files_http3_drain_${mode} PROPERTIES TIMEOUT 25)
    endforeach()
  endif()
endif()
add_executable(http_host_cpp host_cpp_header.cc)
target_link_libraries(http_host_cpp PRIVATE http_test_options CoAkkaHttp::host)
register_test(http_host_cpp)
add_executable(http_fixture_wire fixture_wire_test.c test_wire.c)
target_link_libraries(http_fixture_wire PRIVATE http_test_options CoAkkaHttp::host)
register_test(http_fixture_wire)
if(NOT WIN32)
  add_executable(http_fixture_threads fixture_threads_test.c)
  target_link_libraries(http_fixture_threads PRIVATE http_test_options Threads::Threads)
  add_test(NAME http_fixture_threads COMMAND http_fixture_threads)
  set_tests_properties(http_fixture_threads PROPERTIES TIMEOUT 30)
endif()
if(COAKKA_HTTP_TEST_FUZZ)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "The optional fuzz seam requires Clang libFuzzer")
  endif()
  add_executable(http_fixture_fuzz fixture_wire_fuzz.c test_wire.c)
  target_link_libraries(http_fixture_fuzz PRIVATE http_test_options CoAkkaHttp::host)
  target_compile_options(http_fixture_fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
  target_link_options(http_fixture_fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
endif()
